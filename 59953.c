entry_guards_all_primary_guards_are_down(guard_selection_t *gs)
{
  tor_assert(gs);
  if (!gs->primary_guards_up_to_date)
    entry_guards_update_primary(gs);
  SMARTLIST_FOREACH_BEGIN(gs->primary_entry_guards, entry_guard_t *, guard) {
    entry_guard_consider_retry(guard);
    if (guard->is_reachable != GUARD_REACHABLE_NO)
      return 0;
  } SMARTLIST_FOREACH_END(guard);
  return 1;
}
