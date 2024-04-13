bridge_passes_guard_filter(const or_options_t *options,
                           const bridge_info_t *bridge)
{
  tor_assert(bridge);
  if (!bridge)
    return 0;

  if (routerset_contains_bridge(options->ExcludeNodes, bridge))
    return 0;

  /* Ignore entrynodes */
  const tor_addr_port_t *addrport = bridge_get_addr_port(bridge);

  if (!fascist_firewall_allows_address_addr(&addrport->addr,
                                            addrport->port,
                                            FIREWALL_OR_CONNECTION,
                                            0, 0))
    return 0;

  return 1;
}
