add_unix_port(smartlist_t *ports, rend_service_port_config_t *p)
{
  tor_assert(ports);
  tor_assert(p);
  tor_assert(p->is_unix_addr);

  smartlist_add(ports, p);
  return 0;
}
