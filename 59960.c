entry_guards_note_guard_failure(guard_selection_t *gs,
                                entry_guard_t *guard)
{
  tor_assert(gs);

  guard->is_reachable = GUARD_REACHABLE_NO;
  guard->is_usable_filtered_guard = 0;

  guard->is_pending = 0;
  if (guard->failing_since == 0)
    guard->failing_since = approx_time();

  log_info(LD_GUARD, "Recorded failure for %s%sguard %s",
           guard->is_primary?"primary ":"",
           guard->confirmed_idx>=0?"confirmed ":"",
           entry_guard_describe(guard));
}
