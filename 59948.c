entry_guard_pick_for_circuit(guard_selection_t *gs,
                             guard_usage_t usage,
                             entry_guard_restriction_t *rst,
                             const node_t **chosen_node_out,
                             circuit_guard_state_t **guard_state_out)
{
  tor_assert(gs);
  tor_assert(chosen_node_out);
  tor_assert(guard_state_out);
  *chosen_node_out = NULL;
  *guard_state_out = NULL;

  unsigned state = 0;
  entry_guard_t *guard =
    select_entry_guard_for_circuit(gs, usage, rst, &state);
  if (! guard)
    goto fail;
  if (BUG(state == 0))
    goto fail;
  const node_t *node = node_get_by_id(guard->identity);
  if (! node)
    goto fail;
  if (BUG(usage != GUARD_USAGE_DIRGUARD && !node_has_descriptor(node)))
    goto fail;

  *chosen_node_out = node;
  *guard_state_out = tor_malloc_zero(sizeof(circuit_guard_state_t));
  (*guard_state_out)->guard = entry_guard_handle_new(guard);
  (*guard_state_out)->state = state;
  (*guard_state_out)->state_set_at = approx_time();
  (*guard_state_out)->restrictions = rst;

  return 0;
 fail:
  entry_guard_restriction_free(rst);
  return -1;
}
