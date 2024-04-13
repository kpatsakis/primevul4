rend_service_dump_stats(int severity)
{
  int i,j;
  rend_service_t *service;
  rend_intro_point_t *intro;
  const char *safe_name;
  origin_circuit_t *circ;

  for (i=0; i < smartlist_len(rend_service_list); ++i) {
    service = smartlist_get(rend_service_list, i);
    tor_log(severity, LD_GENERAL, "Service configured in %s:",
            rend_service_escaped_dir(service));
    for (j=0; j < smartlist_len(service->intro_nodes); ++j) {
      intro = smartlist_get(service->intro_nodes, j);
      safe_name = safe_str_client(intro->extend_info->nickname);

      circ = find_intro_circuit(intro, service->pk_digest);
      if (!circ) {
        tor_log(severity, LD_GENERAL, "  Intro point %d at %s: no circuit",
            j, safe_name);
        continue;
      }
      tor_log(severity, LD_GENERAL, "  Intro point %d at %s: circuit is %s",
          j, safe_name, circuit_state_to_string(circ->base_.state));
    }
  }
}
