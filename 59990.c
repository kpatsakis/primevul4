get_sampled_guard_with_id(guard_selection_t *gs,
                          const uint8_t *rsa_id)
{
  tor_assert(gs);
  tor_assert(rsa_id);
  SMARTLIST_FOREACH_BEGIN(gs->sampled_entry_guards, entry_guard_t *, guard) {
    if (tor_memeq(guard->identity, rsa_id, DIGEST_LEN))
      return guard;
  } SMARTLIST_FOREACH_END(guard);
  return NULL;
}
