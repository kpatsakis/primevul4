entry_guards_update_state(or_state_t *state)
{
  entry_guards_dirty = 0;

  entry_guards_update_guards_in_state(state);

  entry_guards_dirty = 0;

  if (!get_options()->AvoidDiskWrites)
    or_state_mark_dirty(get_or_state(), 0);
  entry_guards_dirty = 0;
}
