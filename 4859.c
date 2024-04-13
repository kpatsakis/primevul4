try_build_id_prefix (const char * prefix, char * filename, const unsigned char * data, unsigned long id_len)
{
  char * f = filename;

  f += sprintf (f, "%s.build-id/%02x/", prefix, (unsigned) *data++);
  id_len --;
  while (id_len --)
    f += sprintf (f, "%02x", (unsigned) *data++);
  strcpy (f, ".debug");

  return open_debug_file (filename);
}