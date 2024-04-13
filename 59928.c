circ_state_has_higher_priority(origin_circuit_t *a,
                               const entry_guard_restriction_t *rst,
                               origin_circuit_t *b)
{
  circuit_guard_state_t *state_a = origin_circuit_get_guard_state(a);
  circuit_guard_state_t *state_b = origin_circuit_get_guard_state(b);

  tor_assert(state_a);
  tor_assert(state_b);

  entry_guard_t *guard_a = entry_guard_handle_get(state_a->guard);
  entry_guard_t *guard_b = entry_guard_handle_get(state_b->guard);

  if (! guard_a) {
    /* Unknown guard -- never higher priority. */
    return 0;
  } else if (! guard_b) {
    /* Known guard -- higher priority than any unknown guard. */
    return 1;
  } else  if (! entry_guard_obeys_restriction(guard_a, rst)) {
    /* Restriction violated; guard_a cannot have higher priority. */
    return 0;
  } else {
    /* Both known -- compare.*/
    return entry_guard_has_higher_priority(guard_a, guard_b);
  }
}
