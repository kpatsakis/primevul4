entry_guards_update_filtered_sets(guard_selection_t *gs)
{
  const or_options_t *options = get_options();

  SMARTLIST_FOREACH_BEGIN(gs->sampled_entry_guards, entry_guard_t *, guard) {
    entry_guard_set_filtered_flags(options, gs, guard);
  } SMARTLIST_FOREACH_END(guard);
}
