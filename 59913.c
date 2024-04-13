rend_service_requires_uptime(rend_service_t *service)
{
  int i;
  rend_service_port_config_t *p;

  for (i=0; i < smartlist_len(service->ports); ++i) {
    p = smartlist_get(service->ports, i);
    if (smartlist_contains_int_as_string(get_options()->LongLivedPorts,
                                  p->virtual_port))
      return 1;
  }
  return 0;
}
