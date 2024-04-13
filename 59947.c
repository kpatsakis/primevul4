entry_guard_passes_filter(const or_options_t *options, guard_selection_t *gs,
                          entry_guard_t *guard)
{
  if (guard->currently_listed == 0)
    return 0;
  if (guard->pb.path_bias_disabled)
    return 0;

  if (gs->type == GS_TYPE_BRIDGE) {
    const bridge_info_t *bridge = get_bridge_info_for_guard(guard);
    if (bridge == NULL)
      return 0;
    return bridge_passes_guard_filter(options, bridge);
  } else {
    const node_t *node = node_get_by_id(guard->identity);
    if (node == NULL) {
      return 0;
    }

    return node_passes_guard_filter(options, node);
   }
 }
