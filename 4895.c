check_gnu_debuglink (const char * pathname, void * crc_pointer)
{
  static unsigned char buffer [8 * 1024];
  FILE *         f;
  bfd_size_type  count;
  unsigned long  crc = 0;
  void *         sep_data;

  sep_data = open_debug_file (pathname);
  if (sep_data == NULL)
    return false;

  /* Yes - we are opening the file twice...  */
  f = fopen (pathname, "rb");
  if (f == NULL)
    {
      /* Paranoia: This should never happen.  */
      close_debug_file (sep_data);
      warn (_("Unable to reopen separate debug info file: %s\n"), pathname);
      return false;
    }

  while ((count = fread (buffer, 1, sizeof (buffer), f)) > 0)
    crc = calc_gnu_debuglink_crc32 (crc, buffer, count);

  fclose (f);

  if (crc != * (unsigned long *) crc_pointer)
    {
      close_debug_file (sep_data);
      warn (_("Separate debug info file %s found, but CRC does not match - ignoring\n"),
	    pathname);
      return false;
    }

  return true;
}