create_initial_guard_context(void)
{
  tor_assert(! curr_guard_context);
  if (!guard_contexts) {
    guard_contexts = smartlist_new();
  }
  guard_selection_type_t type = GS_TYPE_INFER;
  const char *name = choose_guard_selection(
                             get_options(),
                             networkstatus_get_live_consensus(approx_time()),
                             NULL,
                             &type);
  tor_assert(name); // "name" can only be NULL if we had an old name.
  tor_assert(type != GS_TYPE_INFER);
  log_notice(LD_GUARD, "Starting with guard context \"%s\"", name);
  curr_guard_context = get_guard_selection_by_name(name, type, 1);
}
