entry_guard_get_by_id_digest(const char *digest)
{
  return entry_guard_get_by_id_digest_for_guard_selection(
      get_guard_selection_info(), digest);
}
