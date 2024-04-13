display_debug_sup (struct dwarf_section *  section,
		   void *                  file ATTRIBUTE_UNUSED)
{
  unsigned char * start = section->start;
  unsigned char * end = section->start + section->size;
  unsigned int version;
  char is_supplementary;
  const unsigned char * sup_filename;
  size_t sup_filename_len;
  unsigned int num_read;
  int status;
  dwarf_vma checksum_len;


  introduce (section, true);
  if (section->size < 4)
    {
      error (_("corrupt .debug_sup section: size is too small\n"));
      return 0;
    }

  /* Read the data.  */
  SAFE_BYTE_GET_AND_INC (version, start, 2, end);
  if (version < 5)
    warn (_("corrupt .debug_sup section: version < 5"));

  SAFE_BYTE_GET_AND_INC (is_supplementary, start, 1, end);
  if (is_supplementary != 0 && is_supplementary != 1)
    warn (_("corrupt .debug_sup section: is_supplementary not 0 or 1\n"));    

  sup_filename = start;
  if (is_supplementary && sup_filename[0] != 0)
    warn (_("corrupt .debug_sup section: filename not empty in supplementary section\n"));

  sup_filename_len = strnlen ((const char *) start, end - start);
  if (sup_filename_len == (size_t) (end - start))
    {
      error (_("corrupt .debug_sup section: filename is not NUL terminated\n"));
      return 0;
    }
  start += sup_filename_len + 1;

  checksum_len = read_leb128 (start, end, false /* unsigned */, & num_read, & status);
  if (status)
    {
      error (_("corrupt .debug_sup section: bad LEB128 field for checksum length\n"));
      checksum_len = 0;
    }
  start += num_read;
  if (checksum_len > (dwarf_vma) (end - start))
    {
      error (_("corrupt .debug_sup section: checksum length is longer than the remaining section length\n"));
      checksum_len = end - start;
    }
  else if (checksum_len < (dwarf_vma) (end - start))
    {
      warn (_("corrupt .debug_sup section: there are 0x%lx extra, unused bytes at the end of the section\n"),
	    (long) ((end - start) - checksum_len));
    }

  printf (_("  Version:      %u\n"), version);
  printf (_("  Is Supp:      %u\n"), is_supplementary);
  printf (_("  Filename:     %s\n"), sup_filename);
  printf (_("  Checksum Len: %lu\n"), (long) checksum_len);
  if (checksum_len > 0)
    {
      printf (_("  Checksum:     "));
      while (checksum_len--)
	printf ("0x%x ", * start++ );
      printf ("\n");
    }
  return 1;
}