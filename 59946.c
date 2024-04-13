entry_guard_parse_from_state(const char *s)
{
  /* Unrecognized entries get put in here. */
  smartlist_t *extra = smartlist_new();

  /* These fields get parsed from the string. */
  char *in = NULL;
  char *rsa_id = NULL;
  char *nickname = NULL;
  char *sampled_on = NULL;
  char *sampled_by = NULL;
  char *unlisted_since = NULL;
  char *listed  = NULL;
  char *confirmed_on = NULL;
  char *confirmed_idx = NULL;
  char *bridge_addr = NULL;

  char *pb_use_attempts = NULL;
  char *pb_use_successes = NULL;
  char *pb_circ_attempts = NULL;
  char *pb_circ_successes = NULL;
  char *pb_successful_circuits_closed = NULL;
  char *pb_collapsed_circuits = NULL;
  char *pb_unusable_circuits = NULL;
  char *pb_timeouts = NULL;

  /* Split up the entries.  Put the ones we know about in strings and the
   * rest in "extra". */
  {
    smartlist_t *entries = smartlist_new();

    strmap_t *vals = strmap_new(); // Maps keyword to location
#define FIELD(f) \
    strmap_set(vals, #f, &f);
    FIELD(in);
    FIELD(rsa_id);
    FIELD(nickname);
    FIELD(sampled_on);
    FIELD(sampled_by);
    FIELD(unlisted_since);
    FIELD(listed);
    FIELD(confirmed_on);
    FIELD(confirmed_idx);
    FIELD(bridge_addr);
    FIELD(pb_use_attempts);
    FIELD(pb_use_successes);
    FIELD(pb_circ_attempts);
    FIELD(pb_circ_successes);
    FIELD(pb_successful_circuits_closed);
    FIELD(pb_collapsed_circuits);
    FIELD(pb_unusable_circuits);
    FIELD(pb_timeouts);
#undef FIELD

    smartlist_split_string(entries, s, " ",
                           SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 0);

    SMARTLIST_FOREACH_BEGIN(entries, char *, entry) {
      const char *eq = strchr(entry, '=');
      if (!eq) {
        smartlist_add(extra, entry);
        continue;
      }
      char *key = tor_strndup(entry, eq-entry);
      char **target = strmap_get(vals, key);
      if (target == NULL || *target != NULL) {
        /* unrecognized or already set */
        smartlist_add(extra, entry);
        tor_free(key);
        continue;
      }

      *target = tor_strdup(eq+1);
      tor_free(key);
      tor_free(entry);
    } SMARTLIST_FOREACH_END(entry);

    smartlist_free(entries);
    strmap_free(vals, NULL);
  }

  entry_guard_t *guard = tor_malloc_zero(sizeof(entry_guard_t));
  guard->is_persistent = 1;

  if (in == NULL) {
    log_warn(LD_CIRC, "Guard missing 'in' field");
    goto err;
  }

  guard->selection_name = in;
  in = NULL;

  if (rsa_id == NULL) {
    log_warn(LD_CIRC, "Guard missing RSA ID field");
    goto err;
  }

  /* Process the identity and nickname. */
  if (base16_decode(guard->identity, sizeof(guard->identity),
                    rsa_id, strlen(rsa_id)) != DIGEST_LEN) {
    log_warn(LD_CIRC, "Unable to decode guard identity %s", escaped(rsa_id));
    goto err;
  }

  if (nickname) {
    strlcpy(guard->nickname, nickname, sizeof(guard->nickname));
  } else {
    guard->nickname[0]='$';
    base16_encode(guard->nickname+1, sizeof(guard->nickname)-1,
                  guard->identity, DIGEST_LEN);
  }

  if (bridge_addr) {
    tor_addr_port_t res;
    memset(&res, 0, sizeof(res));
    int r = tor_addr_port_parse(LOG_WARN, bridge_addr,
                                &res.addr, &res.port, -1);
    if (r == 0)
      guard->bridge_addr = tor_memdup(&res, sizeof(res));
    /* On error, we already warned. */
  }

  /* Process the various time fields. */

#define HANDLE_TIME(field) do {                                 \
    if (field) {                                                \
      int r = parse_iso_time_nospace(field, &field ## _time);   \
      if (r < 0) {                                              \
        log_warn(LD_CIRC, "Unable to parse %s %s from guard",   \
                 #field, escaped(field));                       \
        field##_time = -1;                                      \
      }                                                         \
    }                                                           \
  } while (0)

  time_t sampled_on_time = 0;
  time_t unlisted_since_time = 0;
  time_t confirmed_on_time = 0;

  HANDLE_TIME(sampled_on);
  HANDLE_TIME(unlisted_since);
  HANDLE_TIME(confirmed_on);

  if (sampled_on_time <= 0)
    sampled_on_time = approx_time();
  if (unlisted_since_time < 0)
    unlisted_since_time = 0;
  if (confirmed_on_time < 0)
    confirmed_on_time = 0;

  #undef HANDLE_TIME

  guard->sampled_on_date = sampled_on_time;
  guard->unlisted_since_date = unlisted_since_time;
  guard->confirmed_on_date = confirmed_on_time;

  /* Take sampled_by_version verbatim. */
  guard->sampled_by_version = sampled_by;
  sampled_by = NULL; /* prevent free */

  /* Listed is a boolean */
  if (listed && strcmp(listed, "0"))
    guard->currently_listed = 1;

  /* The index is a nonnegative integer. */
  guard->confirmed_idx = -1;
  if (confirmed_idx) {
    int ok=1;
    long idx = tor_parse_long(confirmed_idx, 10, 0, INT_MAX, &ok, NULL);
    if (! ok) {
      log_warn(LD_GUARD, "Guard has invalid confirmed_idx %s",
               escaped(confirmed_idx));
    } else {
      guard->confirmed_idx = (int)idx;
    }
  }

  /* Anything we didn't recognize gets crammed together */
  if (smartlist_len(extra) > 0) {
    guard->extra_state_fields = smartlist_join_strings(extra, " ", 0, NULL);
  }

  /* initialize non-persistent fields */
  guard->is_reachable = GUARD_REACHABLE_MAYBE;

#define PB_FIELD(field)                                                 \
  do {                                                                  \
    if (pb_ ## field) {                                                 \
      int ok = 1;                                                       \
      double r = tor_parse_double(pb_ ## field, 0.0, 1e9, &ok, NULL);   \
      if (! ok) {                                                       \
        log_warn(LD_CIRC, "Guard has invalid pb_%s %s",                 \
                 #field, pb_ ## field);                                 \
      } else {                                                          \
        guard->pb.field = r;                                            \
      }                                                                 \
    }                                                                   \
  } while (0)
  PB_FIELD(use_attempts);
  PB_FIELD(use_successes);
  PB_FIELD(circ_attempts);
  PB_FIELD(circ_successes);
  PB_FIELD(successful_circuits_closed);
  PB_FIELD(collapsed_circuits);
  PB_FIELD(unusable_circuits);
  PB_FIELD(timeouts);
#undef PB_FIELD

  pathbias_check_use_success_count(guard);
  pathbias_check_close_success_count(guard);

  /* We update everything on this guard later, after we've parsed
   * everything.  */

  goto done;

 err:
  entry_guard_free(guard);
  guard = NULL;

 done:
  tor_free(in);
  tor_free(rsa_id);
  tor_free(nickname);
  tor_free(sampled_on);
  tor_free(sampled_by);
  tor_free(unlisted_since);
  tor_free(listed);
  tor_free(confirmed_on);
  tor_free(confirmed_idx);
  tor_free(bridge_addr);
  tor_free(pb_use_attempts);
  tor_free(pb_use_successes);
  tor_free(pb_circ_attempts);
  tor_free(pb_circ_successes);
  tor_free(pb_successful_circuits_closed);
  tor_free(pb_collapsed_circuits);
  tor_free(pb_unusable_circuits);
  tor_free(pb_timeouts);

  SMARTLIST_FOREACH(extra, char *, cp, tor_free(cp));
  smartlist_free(extra);

  return guard;
}
