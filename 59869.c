remove_invalid_intro_points(rend_service_t *service,
                            smartlist_t *exclude_nodes,
                            smartlist_t *retry_nodes, time_t now)
{
  tor_assert(service);

  /* Remove any expired nodes that doesn't have a circuit. */
  SMARTLIST_FOREACH_BEGIN(service->expiring_nodes, rend_intro_point_t *,
                          intro) {
    origin_circuit_t *intro_circ =
      find_intro_circuit(intro, service->pk_digest);
    if (intro_circ) {
      continue;
    }
    /* No more circuit, cleanup the into point object. */
    SMARTLIST_DEL_CURRENT(service->expiring_nodes, intro);
    rend_intro_point_free(intro);
  } SMARTLIST_FOREACH_END(intro);

  SMARTLIST_FOREACH_BEGIN(service->intro_nodes, rend_intro_point_t *,
                          intro) {
    /* Find the introduction point node object. */
    const node_t *node =
      node_get_by_id(intro->extend_info->identity_digest);
    /* Find the intro circuit, this might be NULL. */
    origin_circuit_t *intro_circ =
      find_intro_circuit(intro, service->pk_digest);

    /* Add the valid node to the exclusion list so we don't try to establish
     * an introduction point to it again. */
    if (node && exclude_nodes) {
      smartlist_add(exclude_nodes, (void*) node);
    }

    /* First, make sure we still have a valid circuit for this intro point.
     * If we dont, we'll give up on it and make a new one. */
    if (intro_circ == NULL) {
      log_info(LD_REND, "Attempting to retry on %s as intro point for %s"
               " (circuit disappeared).",
               safe_str_client(extend_info_describe(intro->extend_info)),
               safe_str_client(service->service_id));
      /* We've lost the circuit for this intro point, flag it so it can be
       * accounted for when considiring uploading a descriptor. */
      intro->circuit_established = 0;

      /* Node is gone or we've reached our maximum circuit creationg retry
       * count, clean up everything, we'll find a new one. */
      if (node == NULL ||
          intro->circuit_retries >= MAX_INTRO_POINT_CIRCUIT_RETRIES) {
        rend_intro_point_free(intro);
        SMARTLIST_DEL_CURRENT(service->intro_nodes, intro);
        /* We've just killed the intro point, nothing left to do. */
        continue;
      }

      /* The intro point is still alive so let's try to use it again because
       * we have a published descriptor containing it. Keep the intro point
       * in the intro_nodes list because it's still valid, we are rebuilding
       * a circuit to it. */
      if (retry_nodes) {
        smartlist_add(retry_nodes, intro);
      }
    }
    /* else, the circuit is valid so in both cases, node being alive or not,
     * we leave the circuit and intro point object as is. Closing the
     * circuit here would leak new consensus timing and freeing the intro
     * point object would make the intro circuit unusable. */

    /* Now, check if intro point should expire. If it does, queue it so
     * it can be cleaned up once it has been replaced properly. */
    if (intro_point_should_expire_now(intro, now)) {
      log_info(LD_REND, "Expiring %s as intro point for %s.",
               safe_str_client(extend_info_describe(intro->extend_info)),
               safe_str_client(service->service_id));
      smartlist_add(service->expiring_nodes, intro);
      SMARTLIST_DEL_CURRENT(service->intro_nodes, intro);
      /* Intro point is expired, we need a new one thus don't consider it
       * anymore has a valid established intro point. */
      intro->circuit_established = 0;
    }
  } SMARTLIST_FOREACH_END(intro);
}
