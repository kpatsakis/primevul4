getinfo_helper_entry_guards(control_connection_t *conn,
                            const char *question, char **answer,
                            const char **errmsg)
{
  guard_selection_t *gs = get_guard_selection_info();

  tor_assert(gs != NULL);

  (void) conn;
  (void) errmsg;

  if (!strcmp(question,"entry-guards") ||
      !strcmp(question,"helper-nodes")) {
    const smartlist_t *guards;
    guards = gs->sampled_entry_guards;

    smartlist_t *sl = smartlist_new();

    SMARTLIST_FOREACH_BEGIN(guards, const entry_guard_t *, e) {
      char *cp = getinfo_helper_format_single_entry_guard(e);
      smartlist_add(sl, cp);
    } SMARTLIST_FOREACH_END(e);
    *answer = smartlist_join_strings(sl, "", 0, NULL);
    SMARTLIST_FOREACH(sl, char *, c, tor_free(c));
    smartlist_free(sl);
  }
  return 0;
}
