circuit_guard_state_free(circuit_guard_state_t *state)
{
  if (!state)
    return;
  entry_guard_restriction_free(state->restrictions);
  entry_guard_handle_free(state->guard);
  tor_free(state);
}
