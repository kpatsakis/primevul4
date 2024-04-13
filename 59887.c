rend_service_del_ephemeral(const char *service_id)
{
  rend_service_t *s;
  if (!rend_valid_service_id(service_id)) {
    log_warn(LD_CONFIG, "Requested malformed Onion Service id for removal.");
    return -1;
  }
  if ((s = rend_service_get_by_service_id(service_id)) == NULL) {
    log_warn(LD_CONFIG, "Requested non-existent Onion Service id for "
             "removal.");
    return -1;
  }
  if (!rend_service_is_ephemeral(s)) {
    log_warn(LD_CONFIG, "Requested non-ephemeral Onion Service for removal.");
    return -1;
  }

  /* Kill the intro point circuit for the Onion Service, and remove it from
   * the list.  Closing existing connections is the application's problem.
   *
   * XXX: As with the comment in rend_config_services(), a nice abstraction
   * would be ideal here, but for now just duplicate the code.
   */
  SMARTLIST_FOREACH_BEGIN(circuit_get_global_list(), circuit_t *, circ) {
    if (!circ->marked_for_close &&
        (circ->purpose == CIRCUIT_PURPOSE_S_ESTABLISH_INTRO ||
         circ->purpose == CIRCUIT_PURPOSE_S_INTRO)) {
      origin_circuit_t *oc = TO_ORIGIN_CIRCUIT(circ);
      tor_assert(oc->rend_data);
      if (!rend_circuit_pk_digest_eq(oc, (uint8_t *) s->pk_digest)) {
        continue;
      }
      log_debug(LD_REND, "Closing intro point %s for service %s.",
                safe_str_client(extend_info_describe(
                                          oc->build_state->chosen_exit)),
                rend_data_get_address(oc->rend_data));
      circuit_mark_for_close(circ, END_CIRC_REASON_FINISHED);
    }
  } SMARTLIST_FOREACH_END(circ);
  smartlist_remove(rend_service_list, s);
  rend_service_free(s);

  log_debug(LD_CONFIG, "Removed ephemeral Onion Service: %s", service_id);

  return 0;
}
