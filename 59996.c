guard_selection_have_enough_dir_info_to_build_circuits(guard_selection_t *gs)
{
  if (!gs->primary_guards_up_to_date)
    entry_guards_update_primary(gs);

  int n_missing_descriptors = 0;
  int n_considered = 0;
  int num_primary_to_check;

  /* We want to check for the descriptor of at least the first two primary
   * guards in our list, since these are the guards that we typically use for
   * circuits. */
  num_primary_to_check = get_n_primary_guards_to_use(GUARD_USAGE_TRAFFIC);
  num_primary_to_check++;

  SMARTLIST_FOREACH_BEGIN(gs->primary_entry_guards, entry_guard_t *, guard) {
    entry_guard_consider_retry(guard);
    if (guard->is_reachable == GUARD_REACHABLE_NO)
      continue;
    n_considered++;
    if (!guard_has_descriptor(guard))
      n_missing_descriptors++;
    if (n_considered >= num_primary_to_check)
      break;
  } SMARTLIST_FOREACH_END(guard);

  return n_missing_descriptors == 0;
}
