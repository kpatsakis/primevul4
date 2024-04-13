get_bridge_info_for_guard(const entry_guard_t *guard)
{
  const uint8_t *identity = NULL;
  if (! tor_digest_is_zero(guard->identity)) {
    identity = (const uint8_t *)guard->identity;
  }
  if (BUG(guard->bridge_addr == NULL))
    return NULL;

  return get_configured_bridge_by_exact_addr_port_digest(
                                                 &guard->bridge_addr->addr,
                                                 guard->bridge_addr->port,
                                                 (const char*)identity);
}
