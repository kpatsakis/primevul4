MOCK_IMPL(STATIC int,
entry_guard_is_listed,(guard_selection_t *gs, const entry_guard_t *guard))
{
  if (gs->type == GS_TYPE_BRIDGE) {
    return NULL != get_bridge_info_for_guard(guard);
  } else {
    const node_t *node = node_get_by_id(guard->identity);

    return node && node_is_possible_guard(node);
  }
}
