entry_guard_add_to_sample_impl(guard_selection_t *gs,
                               const uint8_t *rsa_id_digest,
                               const char *nickname,
                               const tor_addr_port_t *bridge_addrport)
{
  const int GUARD_LIFETIME = get_guard_lifetime();
  tor_assert(gs);


  /* Make sure we can actually identify the guard. */
  if (BUG(!rsa_id_digest && !bridge_addrport))
    return NULL; // LCOV_EXCL_LINE

  entry_guard_t *guard = tor_malloc_zero(sizeof(entry_guard_t));

  /* persistent fields */
  guard->is_persistent = (rsa_id_digest != NULL);
  guard->selection_name = tor_strdup(gs->name);
  if (rsa_id_digest)
    memcpy(guard->identity, rsa_id_digest, DIGEST_LEN);
  if (nickname)
    strlcpy(guard->nickname, nickname, sizeof(guard->nickname));
  guard->sampled_on_date = randomize_time(approx_time(), GUARD_LIFETIME/10);
  tor_free(guard->sampled_by_version);
  guard->sampled_by_version = tor_strdup(VERSION);
  guard->currently_listed = 1;
  guard->confirmed_idx = -1;

  /* non-persistent fields */
  guard->is_reachable = GUARD_REACHABLE_MAYBE;
  if (bridge_addrport)
    guard->bridge_addr = tor_memdup(bridge_addrport, sizeof(*bridge_addrport));

  smartlist_add(gs->sampled_entry_guards, guard);
  guard->in_selection = gs;
  entry_guard_set_filtered_flags(get_options(), gs, guard);
  entry_guards_changed_for_guard_selection(gs);
  return guard;
}
