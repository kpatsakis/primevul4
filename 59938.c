entry_guard_describe(const entry_guard_t *guard)
{
  static char buf[256];
  tor_snprintf(buf, sizeof(buf),
               "%s ($%s)",
               strlen(guard->nickname) ? guard->nickname : "[bridge]",
               hex_str(guard->identity, DIGEST_LEN));
  return buf;
}
