count_intro_point_circuits(const rend_service_t *service)
{
  unsigned int num_ipos = 0;
  SMARTLIST_FOREACH_BEGIN(circuit_get_global_list(), circuit_t *, circ) {
    if (!circ->marked_for_close &&
        circ->state == CIRCUIT_STATE_OPEN &&
        (circ->purpose == CIRCUIT_PURPOSE_S_ESTABLISH_INTRO ||
         circ->purpose == CIRCUIT_PURPOSE_S_INTRO)) {
      origin_circuit_t *oc = TO_ORIGIN_CIRCUIT(circ);
      if (oc->rend_data &&
          rend_circuit_pk_digest_eq(oc, (uint8_t *) service->pk_digest)) {
        num_ipos++;
      }
    }
  }
  SMARTLIST_FOREACH_END(circ);
  return num_ipos;
}
