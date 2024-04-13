entry_guard_succeeded(circuit_guard_state_t **guard_state_p)
{
  if (BUG(*guard_state_p == NULL))
    return GUARD_USABLE_NEVER;

  entry_guard_t *guard = entry_guard_handle_get((*guard_state_p)->guard);
  if (! guard || BUG(guard->in_selection == NULL))
    return GUARD_USABLE_NEVER;

  unsigned newstate =
    entry_guards_note_guard_success(guard->in_selection, guard,
                                    (*guard_state_p)->state);

  (*guard_state_p)->state = newstate;
  (*guard_state_p)->state_set_at = approx_time();

  if (newstate == GUARD_CIRC_STATE_COMPLETE) {
    return GUARD_USABLE_NOW;
  } else {
    return GUARD_MAYBE_USABLE_LATER;
  }
}
