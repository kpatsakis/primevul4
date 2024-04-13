entry_guard_encode_for_state(entry_guard_t *guard)
{
  /*
   * The meta-format we use is K=V K=V K=V... where K can be any
   * characters excepts space and =, and V can be any characters except
   * space.  The order of entries is not allowed to matter.
   * Unrecognized K=V entries are persisted; recognized but erroneous
   * entries are corrected.
   */

  smartlist_t *result = smartlist_new();
  char tbuf[ISO_TIME_LEN+1];

  tor_assert(guard);

  smartlist_add_asprintf(result, "in=%s", guard->selection_name);
  smartlist_add_asprintf(result, "rsa_id=%s",
                         hex_str(guard->identity, DIGEST_LEN));
  if (guard->bridge_addr) {
    smartlist_add_asprintf(result, "bridge_addr=%s:%d",
                           fmt_and_decorate_addr(&guard->bridge_addr->addr),
                           guard->bridge_addr->port);
  }
  if (strlen(guard->nickname) && is_legal_nickname(guard->nickname)) {
    smartlist_add_asprintf(result, "nickname=%s", guard->nickname);
  }

  format_iso_time_nospace(tbuf, guard->sampled_on_date);
  smartlist_add_asprintf(result, "sampled_on=%s", tbuf);

  if (guard->sampled_by_version) {
    smartlist_add_asprintf(result, "sampled_by=%s",
                           guard->sampled_by_version);
  }

  if (guard->unlisted_since_date > 0) {
    format_iso_time_nospace(tbuf, guard->unlisted_since_date);
    smartlist_add_asprintf(result, "unlisted_since=%s", tbuf);
  }

  smartlist_add_asprintf(result, "listed=%d",
                         (int)guard->currently_listed);

  if (guard->confirmed_idx >= 0) {
    format_iso_time_nospace(tbuf, guard->confirmed_on_date);
    smartlist_add_asprintf(result, "confirmed_on=%s", tbuf);

    smartlist_add_asprintf(result, "confirmed_idx=%d", guard->confirmed_idx);
  }

  const double EPSILON = 1.0e-6;

  /* Make a copy of the pathbias object, since we will want to update
     some of them */
  guard_pathbias_t *pb = tor_memdup(&guard->pb, sizeof(*pb));
  pb->use_successes = pathbias_get_use_success_count(guard);
  pb->successful_circuits_closed = pathbias_get_close_success_count(guard);

  #define PB_FIELD(field) do {                                          \
      if (pb->field >= EPSILON) {                                       \
        smartlist_add_asprintf(result, "pb_" #field "=%f", pb->field);  \
      }                                                                 \
    } while (0)
  PB_FIELD(use_attempts);
  PB_FIELD(use_successes);
  PB_FIELD(circ_attempts);
  PB_FIELD(circ_successes);
  PB_FIELD(successful_circuits_closed);
  PB_FIELD(collapsed_circuits);
  PB_FIELD(unusable_circuits);
  PB_FIELD(timeouts);
  tor_free(pb);
#undef PB_FIELD

  if (guard->extra_state_fields)
    smartlist_add_strdup(result, guard->extra_state_fields);

  char *joined = smartlist_join_strings(result, " ", 0, NULL);
  SMARTLIST_FOREACH(result, char *, cp, tor_free(cp));
  smartlist_free(result);

  return joined;
}
