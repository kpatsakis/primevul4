entry_guard_state_should_expire(circuit_guard_state_t *guard_state)
{
  if (guard_state == NULL)
    return 0;
  const time_t expire_if_waiting_since =
    approx_time() - get_nonprimary_guard_idle_timeout();
  return (guard_state->state == GUARD_CIRC_STATE_WAITING_FOR_BETTER_GUARD
          && guard_state->state_set_at < expire_if_waiting_since);
}
