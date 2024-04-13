entry_guards_load_guards_from_state(or_state_t *state, int set)
{
  const config_line_t *line = state->Guard;
  int n_errors = 0;

  if (!guard_contexts)
    guard_contexts = smartlist_new();

  /* Wipe all our existing guard info. (we shouldn't have any, but
   * let's be safe.) */
  if (set) {
    SMARTLIST_FOREACH_BEGIN(guard_contexts, guard_selection_t *, gs) {
      guard_selection_free(gs);
      if (curr_guard_context == gs)
        curr_guard_context = NULL;
      SMARTLIST_DEL_CURRENT(guard_contexts, gs);
    } SMARTLIST_FOREACH_END(gs);
  }

  for ( ; line != NULL; line = line->next) {
    entry_guard_t *guard = entry_guard_parse_from_state(line->value);
    if (guard == NULL) {
      ++n_errors;
      continue;
    }
    tor_assert(guard->selection_name);
    if (!strcmp(guard->selection_name, "legacy")) {
      ++n_errors;
      entry_guard_free(guard);
      continue;
    }

    if (set) {
      guard_selection_t *gs;
      gs = get_guard_selection_by_name(guard->selection_name,
                                       GS_TYPE_INFER, 1);
      tor_assert(gs);
      smartlist_add(gs->sampled_entry_guards, guard);
      guard->in_selection = gs;
    } else {
      entry_guard_free(guard);
    }
  }

  if (set) {
    SMARTLIST_FOREACH_BEGIN(guard_contexts, guard_selection_t *, gs) {
      entry_guards_update_all(gs);
    } SMARTLIST_FOREACH_END(gs);
  }
  return n_errors ? -1 : 0;
}
