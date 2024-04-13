guard_selection_free(guard_selection_t *gs)
{
  if (!gs) return;

  tor_free(gs->name);

  if (gs->sampled_entry_guards) {
    SMARTLIST_FOREACH(gs->sampled_entry_guards, entry_guard_t *, e,
                      entry_guard_free(e));
    smartlist_free(gs->sampled_entry_guards);
    gs->sampled_entry_guards = NULL;
  }

  smartlist_free(gs->confirmed_entry_guards);
  smartlist_free(gs->primary_entry_guards);

  tor_free(gs);
}
