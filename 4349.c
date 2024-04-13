update_map (char * const mapping, const char * const map_file)
{
  const size_t map_len = strlen (mapping);

  const int fd = xopen (map_file, O_WRONLY, 0);
  xwrite (fd, mapping, map_len);
  xclose (fd);
}