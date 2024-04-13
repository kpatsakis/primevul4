entry_guard_get_by_id_digest_for_guard_selection(guard_selection_t *gs,
                                                 const char *digest)
{
  return get_sampled_guard_with_id(gs, (const uint8_t*)digest);
}
