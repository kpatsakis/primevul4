entry_guards_update_primary(guard_selection_t *gs)
{
  tor_assert(gs);

  static int running = 0;
  tor_assert(!running);
  running = 1;

  const int N_PRIMARY_GUARDS = get_n_primary_guards();

  smartlist_t *new_primary_guards = smartlist_new();
  smartlist_t *old_primary_guards = smartlist_new();
  smartlist_add_all(old_primary_guards, gs->primary_entry_guards);

  /* Set this flag now, to prevent the calls below from recursing. */
  gs->primary_guards_up_to_date = 1;

  /* First, can we fill it up with confirmed guards? */
  SMARTLIST_FOREACH_BEGIN(gs->confirmed_entry_guards, entry_guard_t *, guard) {
    if (smartlist_len(new_primary_guards) >= N_PRIMARY_GUARDS)
      break;
    if (! guard->is_filtered_guard)
      continue;
    guard->is_primary = 1;
    smartlist_add(new_primary_guards, guard);
  } SMARTLIST_FOREACH_END(guard);

  /* Can we keep any older primary guards? First remove all the ones
   * that we already kept. */
  SMARTLIST_FOREACH_BEGIN(old_primary_guards, entry_guard_t *, guard) {
    if (smartlist_contains(new_primary_guards, guard)) {
      SMARTLIST_DEL_CURRENT_KEEPORDER(old_primary_guards, guard);
    }
  } SMARTLIST_FOREACH_END(guard);

  /* Now add any that are still good. */
  SMARTLIST_FOREACH_BEGIN(old_primary_guards, entry_guard_t *, guard) {
    if (smartlist_len(new_primary_guards) >= N_PRIMARY_GUARDS)
      break;
    if (! guard->is_filtered_guard)
      continue;
    guard->is_primary = 1;
    smartlist_add(new_primary_guards, guard);
    SMARTLIST_DEL_CURRENT_KEEPORDER(old_primary_guards, guard);
  } SMARTLIST_FOREACH_END(guard);

  /* Mark the remaining previous primary guards as non-primary */
  SMARTLIST_FOREACH_BEGIN(old_primary_guards, entry_guard_t *, guard) {
    guard->is_primary = 0;
  } SMARTLIST_FOREACH_END(guard);

  /* Finally, fill out the list with sampled guards. */
  while (smartlist_len(new_primary_guards) < N_PRIMARY_GUARDS) {
    entry_guard_t *guard = sample_reachable_filtered_entry_guards(gs, NULL,
                                            SAMPLE_EXCLUDE_CONFIRMED|
                                            SAMPLE_EXCLUDE_PRIMARY|
                                            SAMPLE_NO_UPDATE_PRIMARY);
    if (!guard)
      break;
    guard->is_primary = 1;
    smartlist_add(new_primary_guards, guard);
  }

#if 1
  /* Debugging. */
  SMARTLIST_FOREACH(gs->sampled_entry_guards, entry_guard_t *, guard, {
    tor_assert_nonfatal(
                   bool_eq(guard->is_primary,
                           smartlist_contains(new_primary_guards, guard)));
  });
#endif

  int any_change = 0;
  if (smartlist_len(gs->primary_entry_guards) !=
      smartlist_len(new_primary_guards)) {
    any_change = 1;
  } else {
    SMARTLIST_FOREACH_BEGIN(gs->primary_entry_guards, entry_guard_t *, g) {
      if (g != smartlist_get(new_primary_guards, g_sl_idx)) {
        any_change = 1;
      }
    } SMARTLIST_FOREACH_END(g);
  }

  if (any_change) {
    log_info(LD_GUARD, "Primary entry guards have changed. "
             "New primary guard list is: ");
    int n = smartlist_len(new_primary_guards);
    SMARTLIST_FOREACH_BEGIN(new_primary_guards, entry_guard_t *, g) {
      log_info(LD_GUARD, "  %d/%d: %s%s%s",
               g_sl_idx+1, n, entry_guard_describe(g),
               g->confirmed_idx >= 0 ? " (confirmed)" : "",
               g->is_filtered_guard ? "" : " (excluded by filter)");
    } SMARTLIST_FOREACH_END(g);
  }

  smartlist_free(old_primary_guards);
  smartlist_free(gs->primary_entry_guards);
  gs->primary_entry_guards = new_primary_guards;
  gs->primary_guards_up_to_date = 1;
  running = 0;
}
