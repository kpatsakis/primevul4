entry_guards_update_all(guard_selection_t *gs)
{
  sampled_guards_update_from_consensus(gs);
  entry_guards_update_filtered_sets(gs);
  entry_guards_update_confirmed(gs);
  entry_guards_update_primary(gs);
  return 0;
}
