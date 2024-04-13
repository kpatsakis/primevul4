entry_guard_cancel(circuit_guard_state_t **guard_state_p)
{
  if (BUG(*guard_state_p == NULL))
    return;
  entry_guard_t *guard = entry_guard_handle_get((*guard_state_p)->guard);
  if (! guard)
    return;

  /* XXXX prop271 -- last_tried_to_connect_at will be erroneous here, but this
   * function will only get called in "bug" cases anyway. */
  guard->is_pending = 0;
  circuit_guard_state_free(*guard_state_p);
  *guard_state_p = NULL;
}
