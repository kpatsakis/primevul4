get_sampled_guard_for_bridge(guard_selection_t *gs,
                             const bridge_info_t *bridge)
{
  const uint8_t *id = bridge_get_rsa_id_digest(bridge);
  const tor_addr_port_t *addrport = bridge_get_addr_port(bridge);
  entry_guard_t *guard;
  if (BUG(!addrport))
    return NULL; // LCOV_EXCL_LINE
  guard = get_sampled_guard_by_bridge_addr(gs, addrport);
  if (! guard || (id && tor_memneq(id, guard->identity, DIGEST_LEN)))
    return NULL;
  else
    return guard;
}
