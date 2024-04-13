rend_consider_services_intro_points(void)
{
  int i;
  time_t now;
  const or_options_t *options = get_options();
  /* Are we in single onion mode? */
  const int allow_direct = rend_service_allow_non_anonymous_connection(
                                                                get_options());
  /* List of nodes we need to _exclude_ when choosing a new node to
   * establish an intro point to. */
  smartlist_t *exclude_nodes;
  /* List of nodes we need to retry to build a circuit on them because the
   * node is valid but circuit died. */
  smartlist_t *retry_nodes;

  if (!have_completed_a_circuit())
    return;

  exclude_nodes = smartlist_new();
  retry_nodes = smartlist_new();
  now = time(NULL);

  SMARTLIST_FOREACH_BEGIN(rend_service_list, rend_service_t *, service) {
    int r;
    /* Number of intro points we want to open and add to the intro nodes
     * list of the service. */
    unsigned int n_intro_points_to_open;
    /* Have an unsigned len so we can use it to compare values else gcc is
     * not happy with unmatching signed comparaison. */
    unsigned int intro_nodes_len;
    /* Different service are allowed to have the same introduction point as
     * long as they are on different circuit thus why we clear this list. */
    smartlist_clear(exclude_nodes);
    smartlist_clear(retry_nodes);

    /* Cleanup the invalid intro points and save the node objects, if any,
     * in the exclude_nodes and retry_nodes lists. */
    remove_invalid_intro_points(service, exclude_nodes, retry_nodes, now);

    /* This retry period is important here so we don't stress circuit
     * creation. */
    if (now > service->intro_period_started + INTRO_CIRC_RETRY_PERIOD) {
      /* One period has elapsed; we can try building circuits again. */
      service->intro_period_started = now;
      service->n_intro_circuits_launched = 0;
    } else if (service->n_intro_circuits_launched >=
               rend_max_intro_circs_per_period(
                                      service->n_intro_points_wanted)) {
      /* We have failed too many times in this period; wait for the next
       * one before we try to initiate any more connections. */
      continue;
    }

    /* Let's try to rebuild circuit on the nodes we want to retry on. */
    SMARTLIST_FOREACH_BEGIN(retry_nodes, rend_intro_point_t *, intro) {
      r = rend_service_launch_establish_intro(service, intro);
      if (r < 0) {
        log_warn(LD_REND, "Error launching circuit to node %s for service %s.",
                 safe_str_client(extend_info_describe(intro->extend_info)),
                 safe_str_client(service->service_id));
        /* Unable to launch a circuit to that intro point, remove it from
         * the valid list so we can create a new one. */
        smartlist_remove(service->intro_nodes, intro);
        rend_intro_point_free(intro);
        continue;
      }
      intro->circuit_retries++;
    } SMARTLIST_FOREACH_END(intro);

    /* Avoid mismatched signed comparaison below. */
    intro_nodes_len = (unsigned int) smartlist_len(service->intro_nodes);

    /* Quiescent state, we have more or the equal amount of wanted node for
     * this service. Proceed to the next service. We can have more nodes
     * because we launch extra preemptive circuits if our intro nodes list was
     * originally empty for performance reasons. */
    if (intro_nodes_len >= service->n_intro_points_wanted) {
      continue;
    }

    /* Number of intro points we want to open which is the wanted amount minus
     * the current amount of valid nodes. We know that this won't underflow
     * because of the check above. */
    n_intro_points_to_open = service->n_intro_points_wanted - intro_nodes_len;
    if (intro_nodes_len == 0) {
      /* We want to end up with n_intro_points_wanted intro points, but if
       * we have no intro points at all (chances are they all cycled or we
       * are starting up), we launch NUM_INTRO_POINTS_EXTRA extra circuits
       * and use the first n_intro_points_wanted that complete. See proposal
       * #155, section 4 for the rationale of this which is purely for
       * performance.
       *
       * The ones after the first n_intro_points_to_open will be converted
       * to 'general' internal circuits in rend_service_intro_has_opened(),
       * and then we'll drop them from the list of intro points. */
      n_intro_points_to_open += NUM_INTRO_POINTS_EXTRA;
    }

    for (i = 0; i < (int) n_intro_points_to_open; i++) {
      const node_t *node;
      rend_intro_point_t *intro;
      router_crn_flags_t flags = CRN_NEED_UPTIME|CRN_NEED_DESC;
      if (get_options()->AllowInvalid_ & ALLOW_INVALID_INTRODUCTION)
        flags |= CRN_ALLOW_INVALID;
      router_crn_flags_t direct_flags = flags;
      direct_flags |= CRN_PREF_ADDR;
      direct_flags |= CRN_DIRECT_CONN;

      node = router_choose_random_node(exclude_nodes,
                                       options->ExcludeNodes,
                                       allow_direct ? direct_flags : flags);
      /* If we are in single onion mode, retry node selection for a 3-hop
       * path */
      if (allow_direct && !node) {
        log_info(LD_REND,
                 "Unable to find an intro point that we can connect to "
                 "directly for %s, falling back to a 3-hop path.",
                 safe_str_client(service->service_id));
        node = router_choose_random_node(exclude_nodes,
                                         options->ExcludeNodes, flags);
      }

      if (!node) {
        log_warn(LD_REND,
                 "We only have %d introduction points established for %s; "
                 "wanted %u.",
                 smartlist_len(service->intro_nodes),
                 safe_str_client(service->service_id),
                 n_intro_points_to_open);
        break;
      }
      /* Add the choosen node to the exclusion list in order to avoid picking
       * it again in the next iteration. */
      smartlist_add(exclude_nodes, (void*)node);
      intro = tor_malloc_zero(sizeof(rend_intro_point_t));
      /* extend_info is for clients, so we want the multi-hop primary ORPort,
       * even if we are a single onion service and intend to connect to it
       * directly ourselves. */
      intro->extend_info = extend_info_from_node(node, 0);
      if (BUG(intro->extend_info == NULL)) {
        break;
      }
      intro->intro_key = crypto_pk_new();
      const int fail = crypto_pk_generate_key(intro->intro_key);
      tor_assert(!fail);
      intro->time_published = -1;
      intro->time_to_expire = -1;
      intro->max_introductions =
        crypto_rand_int_range(INTRO_POINT_MIN_LIFETIME_INTRODUCTIONS,
                              INTRO_POINT_MAX_LIFETIME_INTRODUCTIONS);
      smartlist_add(service->intro_nodes, intro);
      log_info(LD_REND, "Picked router %s as an intro point for %s.",
               safe_str_client(node_describe(node)),
               safe_str_client(service->service_id));
      /* Establish new introduction circuit to our chosen intro point. */
      r = rend_service_launch_establish_intro(service, intro);
      if (r < 0) {
        log_warn(LD_REND, "Error launching circuit to node %s for service %s.",
                 safe_str_client(extend_info_describe(intro->extend_info)),
                 safe_str_client(service->service_id));
        /* This funcion will be called again by the main loop so this intro
         * point without a intro circuit will be retried on or removed after
         * a maximum number of attempts. */
      }
    }
  } SMARTLIST_FOREACH_END(service);
  smartlist_free(exclude_nodes);
  smartlist_free(retry_nodes);
}
