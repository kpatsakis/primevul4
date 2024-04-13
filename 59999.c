guards_choose_dirguard(circuit_guard_state_t **guard_state_out)
{
  const node_t *r = NULL;
  if (entry_guard_pick_for_circuit(get_guard_selection_info(),
                                   GUARD_USAGE_DIRGUARD,
                                   NULL,
                                   &r,
                                   guard_state_out) < 0) {
    tor_assert(r == NULL);
  }
  return r;
}
