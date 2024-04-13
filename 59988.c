get_sampled_guard_by_bridge_addr(guard_selection_t *gs,
                                 const tor_addr_port_t *addrport)
{
  if (! gs)
    return NULL;
  if (BUG(!addrport))
    return NULL;
  SMARTLIST_FOREACH_BEGIN(gs->sampled_entry_guards, entry_guard_t *, g) {
    if (g->bridge_addr && tor_addr_port_eq(addrport, g->bridge_addr))
      return g;
  } SMARTLIST_FOREACH_END(g);
  return NULL;
}
