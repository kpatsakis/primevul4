fetch_indirect_string (dwarf_vma offset)
{
  struct dwarf_section *section = &debug_displays [str].section;
  const unsigned char * ret;

  if (section->start == NULL)
    return (const unsigned char *) _("<no .debug_str section>");

  if (offset >= section->size)
    {
      warn (_("DW_FORM_strp offset too big: 0x%s\n"),
	    dwarf_vmatoa ("x", offset));
      return (const unsigned char *) _("<offset is too big>");
    }

  ret = section->start + offset;
  /* Unfortunately we cannot rely upon the .debug_str section ending with a
     NUL byte.  Since our caller is expecting to receive a well formed C
     string we test for the lack of a terminating byte here.  */
  if (strnlen ((const char *) ret, section->size - offset)
      == section->size - offset)
    ret = (const unsigned char *)
      _("<no NUL byte at end of .debug_str section>");

  return ret;
}