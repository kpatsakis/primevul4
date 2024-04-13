guard_selection_new(const char *name,
                    guard_selection_type_t type)
{
  guard_selection_t *gs;

  type = guard_selection_infer_type(type, name);

  gs = tor_malloc_zero(sizeof(*gs));
  gs->name = tor_strdup(name);
  gs->type = type;
  gs->sampled_entry_guards = smartlist_new();
  gs->confirmed_entry_guards = smartlist_new();
  gs->primary_entry_guards = smartlist_new();

  return gs;
}
