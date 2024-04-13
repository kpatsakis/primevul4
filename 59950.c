entry_guard_set_filtered_flags(const or_options_t *options,
                               guard_selection_t *gs,
                               entry_guard_t *guard)
{
  unsigned was_filtered = guard->is_filtered_guard;
  guard->is_filtered_guard = 0;
  guard->is_usable_filtered_guard = 0;

  if (entry_guard_passes_filter(options, gs, guard)) {
    guard->is_filtered_guard = 1;

    if (guard->is_reachable != GUARD_REACHABLE_NO)
      guard->is_usable_filtered_guard = 1;

    entry_guard_consider_retry(guard);
  }
  log_debug(LD_GUARD, "Updated sampled guard %s: filtered=%d; "
            "reachable_filtered=%d.", entry_guard_describe(guard),
            guard->is_filtered_guard, guard->is_usable_filtered_guard);

  if (!bool_eq(was_filtered, guard->is_filtered_guard)) {
    /* This guard might now be primary or nonprimary. */
    gs->primary_guards_up_to_date = 0;
  }
}
