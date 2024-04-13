find_expiring_intro_point(rend_service_t *service, origin_circuit_t *circ)
{
  tor_assert(service);
  tor_assert(TO_CIRCUIT(circ)->purpose == CIRCUIT_PURPOSE_S_ESTABLISH_INTRO ||
             TO_CIRCUIT(circ)->purpose == CIRCUIT_PURPOSE_S_INTRO);

  SMARTLIST_FOREACH(service->expiring_nodes, rend_intro_point_t *,
                    intro_point,
    if (crypto_pk_eq_keys(intro_point->intro_key, circ->intro_key)) {
      return intro_point;
  });

  return NULL;
}
