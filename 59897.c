rend_service_launch_establish_intro(rend_service_t *service,
                                    rend_intro_point_t *intro)
{
  origin_circuit_t *launched;
  int flags = CIRCLAUNCH_NEED_UPTIME|CIRCLAUNCH_IS_INTERNAL;
  const or_options_t *options = get_options();
  extend_info_t *launch_ei = intro->extend_info;
  extend_info_t *direct_ei = NULL;

  /* Are we in single onion mode? */
  if (rend_service_allow_non_anonymous_connection(options)) {
    /* Do we have a descriptor for the node?
     * We've either just chosen it from the consensus, or we've just reviewed
     * our intro points to see which ones are still valid, and deleted the ones
     * that aren't in the consensus any more. */
    const node_t *node = node_get_by_id(launch_ei->identity_digest);
    if (BUG(!node)) {
      /* The service has kept an intro point after it went missing from the
       * consensus. If we did anything else here, it would be a consensus
       * distinguisher. Which are less of an issue for single onion services,
       * but still a bug. */
      return -1;
    }
    /* Can we connect to the node directly? If so, replace launch_ei
     * (a multi-hop extend_info) with one suitable for direct connection. */
    if (rend_service_use_direct_connection_node(options, node)) {
      direct_ei = extend_info_from_node(node, 1);
      if (BUG(!direct_ei)) {
        /* rend_service_use_direct_connection_node and extend_info_from_node
         * disagree about which addresses on this node are permitted. This
         * should never happen. Avoiding the connection is a safe response. */
        return -1;
      }
      flags = flags | CIRCLAUNCH_ONEHOP_TUNNEL;
      launch_ei = direct_ei;
    }
  }
  /* launch_ei is either intro->extend_info, or has been replaced with a valid
   * extend_info for single onion service direct connection. */
  tor_assert(launch_ei);
  /* We must have the same intro when making a direct connection. */
  tor_assert(tor_memeq(intro->extend_info->identity_digest,
                       launch_ei->identity_digest,
                       DIGEST_LEN));

  log_info(LD_REND,
           "Launching circuit to introduction point %s%s%s for service %s",
           safe_str_client(extend_info_describe(intro->extend_info)),
           direct_ei ? " via direct address " : "",
           direct_ei ? safe_str_client(extend_info_describe(direct_ei)) : "",
           service->service_id);

  rep_hist_note_used_internal(time(NULL), 1, 0);

  ++service->n_intro_circuits_launched;
  launched = circuit_launch_by_extend_info(CIRCUIT_PURPOSE_S_ESTABLISH_INTRO,
                             launch_ei, flags);

  if (!launched) {
    log_info(LD_REND,
             "Can't launch circuit to establish introduction at %s%s%s.",
             safe_str_client(extend_info_describe(intro->extend_info)),
             direct_ei ? " via direct address " : "",
             direct_ei ? safe_str_client(extend_info_describe(direct_ei)) : ""
             );
    extend_info_free(direct_ei);
    return -1;
  }
  /* We must have the same exit node even if cannibalized or direct connection.
   */
  tor_assert(tor_memeq(intro->extend_info->identity_digest,
                       launched->build_state->chosen_exit->identity_digest,
                       DIGEST_LEN));

  launched->rend_data = rend_data_service_create(service->service_id,
                                                 service->pk_digest, NULL,
                                                 service->auth_type);
  launched->intro_key = crypto_pk_dup_key(intro->intro_key);
  if (launched->base_.state == CIRCUIT_STATE_OPEN)
    rend_service_intro_has_opened(launched);
  extend_info_free(direct_ei);
  return 0;
}
