choose_guard_selection(const or_options_t *options,
                       const networkstatus_t *live_ns,
                       const guard_selection_t *old_selection,
                       guard_selection_type_t *type_out)
{
  tor_assert(options);
  tor_assert(type_out);

  if (options->UseBridges) {
    *type_out = GS_TYPE_BRIDGE;
    return "bridges";
  }

  if (! live_ns) {
    /* without a networkstatus, we can't tell any more than that. */
    *type_out = GS_TYPE_NORMAL;
    return "default";
  }

  const smartlist_t *nodes = nodelist_get_list();
  int n_guards = 0, n_passing_filter = 0;
  SMARTLIST_FOREACH_BEGIN(nodes, const node_t *, node) {
    if (node_is_possible_guard(node)) {
      ++n_guards;
      if (node_passes_guard_filter(options, node)) {
        ++n_passing_filter;
      }
    }
  } SMARTLIST_FOREACH_END(node);

  /* We use separate 'high' and 'low' thresholds here to prevent flapping
   * back and forth */
  const int meaningful_threshold_high =
    (int)(n_guards * get_meaningful_restriction_threshold() * 1.05);
  const int meaningful_threshold_mid =
    (int)(n_guards * get_meaningful_restriction_threshold());
  const int meaningful_threshold_low =
    (int)(n_guards * get_meaningful_restriction_threshold() * .95);
  const int extreme_threshold =
    (int)(n_guards * get_extreme_restriction_threshold());

  /*
    If we have no previous selection, then we're "restricted" iff we are
    below the meaningful restriction threshold.  That's easy enough.

    But if we _do_ have a previous selection, we make it a little
    "sticky": we only move from "restricted" to "default" when we find
    that we're above the threshold plus 5%, and we only move from
    "default" to "restricted" when we're below the threshold minus 5%.
    That should prevent us from flapping back and forth if we happen to
    be hovering very close to the default.

    The extreme threshold is for warning only.
  */

  static int have_warned_extreme_threshold = 0;
  if (n_guards &&
      n_passing_filter < extreme_threshold &&
      ! have_warned_extreme_threshold) {
    have_warned_extreme_threshold = 1;
    const double exclude_frac =
      (n_guards - n_passing_filter) / (double)n_guards;
    log_warn(LD_GUARD, "Your configuration excludes %d%% of all possible "
             "guards. That's likely to make you stand out from the "
             "rest of the world.", (int)(exclude_frac * 100));
  }

  /* Easy case: no previous selection. Just check if we are in restricted or
     normal guard selection. */
  if (old_selection == NULL) {
    if (n_passing_filter >= meaningful_threshold_mid) {
      *type_out = GS_TYPE_NORMAL;
      return "default";
    } else {
      *type_out = GS_TYPE_RESTRICTED;
      return "restricted";
    }
  }

  /* Trickier case: we do have a previous guard selection context. */
  tor_assert(old_selection);

  /* Use high and low thresholds to decide guard selection, and if we fall in
     the middle then keep the current guard selection context. */
  if (n_passing_filter >= meaningful_threshold_high) {
    *type_out = GS_TYPE_NORMAL;
    return "default";
  } else if (n_passing_filter < meaningful_threshold_low) {
    *type_out = GS_TYPE_RESTRICTED;
    return "restricted";
  } else {
    /* we are in the middle: maintain previous guard selection */
    *type_out = old_selection->type;
    return old_selection->name;
  }
}
