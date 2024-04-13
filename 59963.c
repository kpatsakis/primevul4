entry_guards_parse_state(or_state_t *state, int set, char **msg)
{
  entry_guards_dirty = 0;
  int r1 = entry_guards_load_guards_from_state(state, set);
  entry_guards_dirty = 0;

  if (r1 < 0) {
    if (msg && *msg == NULL) {
      *msg = tor_strdup("parsing error");
    }
    return -1;
  }
  return 0;
}
