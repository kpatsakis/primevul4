entry_guard_add_bridge_to_sample(guard_selection_t *gs,
                                 const bridge_info_t *bridge)
{
  const uint8_t *id_digest = bridge_get_rsa_id_digest(bridge);
  const tor_addr_port_t *addrport = bridge_get_addr_port(bridge);

  tor_assert(addrport);

  /* make sure that the guard is not already sampled. */
  if (BUG(get_sampled_guard_for_bridge(gs, bridge)))
    return NULL; // LCOV_EXCL_LINE

  return entry_guard_add_to_sample_impl(gs, id_digest, NULL, addrport);
}
