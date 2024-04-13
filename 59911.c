rend_service_relaunch_rendezvous(origin_circuit_t *oldcirc)
{
  origin_circuit_t *newcirc;
  cpath_build_state_t *newstate, *oldstate;

  tor_assert(oldcirc->base_.purpose == CIRCUIT_PURPOSE_S_CONNECT_REND);

  /* Don't relaunch the same rend circ twice. */
  if (oldcirc->hs_service_side_rend_circ_has_been_relaunched) {
    log_info(LD_REND, "Rendezvous circuit to %s has already been relaunched; "
             "not relaunching it again.",
             oldcirc->build_state ?
             safe_str(extend_info_describe(oldcirc->build_state->chosen_exit))
             : "*unknown*");
    return;
  }
  oldcirc->hs_service_side_rend_circ_has_been_relaunched = 1;

  if (!oldcirc->build_state ||
      oldcirc->build_state->failure_count > MAX_REND_FAILURES ||
      oldcirc->build_state->expiry_time < time(NULL)) {
    log_info(LD_REND,
             "Attempt to build circuit to %s for rendezvous has failed "
             "too many times or expired; giving up.",
             oldcirc->build_state ?
             safe_str(extend_info_describe(oldcirc->build_state->chosen_exit))
             : "*unknown*");
    return;
  }

  oldstate = oldcirc->build_state;
  tor_assert(oldstate);

  if (oldstate->service_pending_final_cpath_ref == NULL) {
    log_info(LD_REND,"Skipping relaunch of circ that failed on its first hop. "
             "Initiator will retry.");
    return;
  }

  log_info(LD_REND,"Reattempting rendezvous circuit to '%s'",
           safe_str(extend_info_describe(oldstate->chosen_exit)));

  /* You'd think Single Onion Services would want to retry the rendezvous
   * using a direct connection. But if it's blocked by a firewall, or the
   * service is IPv6-only, or the rend point avoiding becoming a one-hop
   * proxy, we need a 3-hop connection. */
  newcirc = circuit_launch_by_extend_info(CIRCUIT_PURPOSE_S_CONNECT_REND,
                            oldstate->chosen_exit,
                            CIRCLAUNCH_NEED_CAPACITY|CIRCLAUNCH_IS_INTERNAL);

  if (!newcirc) {
    log_warn(LD_REND,"Couldn't relaunch rendezvous circuit to '%s'.",
             safe_str(extend_info_describe(oldstate->chosen_exit)));
    return;
  }
  newstate = newcirc->build_state;
  tor_assert(newstate);
  newstate->failure_count = oldstate->failure_count+1;
  newstate->expiry_time = oldstate->expiry_time;
  newstate->service_pending_final_cpath_ref =
    oldstate->service_pending_final_cpath_ref;
  ++(newstate->service_pending_final_cpath_ref->refcount);

  newcirc->rend_data = rend_data_dup(oldcirc->rend_data);
}
