entry_guards_update_guards_in_state(or_state_t *state)
{
  if (!guard_contexts)
    return;
  config_line_t *lines = NULL;
  config_line_t **nextline = &lines;

  SMARTLIST_FOREACH_BEGIN(guard_contexts, guard_selection_t *, gs) {
    SMARTLIST_FOREACH_BEGIN(gs->sampled_entry_guards, entry_guard_t *, guard) {
      if (guard->is_persistent == 0)
        continue;
      *nextline = tor_malloc_zero(sizeof(config_line_t));
      (*nextline)->key = tor_strdup("Guard");
      (*nextline)->value = entry_guard_encode_for_state(guard);
      nextline = &(*nextline)->next;
    } SMARTLIST_FOREACH_END(guard);
  } SMARTLIST_FOREACH_END(gs);

  config_free_lines(state->Guard);
  state->Guard = lines;
}
