rend_service_desc_has_uploaded(const rend_data_t *rend_data)
{
  rend_service_t *service;
  const char *onion_address;

  tor_assert(rend_data);

  onion_address = rend_data_get_address(rend_data);

  service = rend_service_get_by_service_id(onion_address);
  if (service == NULL) {
    return;
  }

  SMARTLIST_FOREACH_BEGIN(service->expiring_nodes, rend_intro_point_t *,
                          intro) {
    origin_circuit_t *intro_circ =
      find_intro_circuit(intro, service->pk_digest);
    if (intro_circ != NULL) {
      circuit_mark_for_close(TO_CIRCUIT(intro_circ),
                             END_CIRC_REASON_FINISHED);
    }
    SMARTLIST_DEL_CURRENT(service->expiring_nodes, intro);
    rend_intro_point_free(intro);
  } SMARTLIST_FOREACH_END(intro);
}
