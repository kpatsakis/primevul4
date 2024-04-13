fetch_alt_indirect_string (dwarf_vma offset)
{
  separate_info * i;

  if (! do_follow_links)
    return "";

  if (first_separate_info == NULL)
    return _("<no links available>");

  for (i = first_separate_info; i != NULL; i = i->next)
    {
      struct dwarf_section * section;
      const char *           ret;

      if (! load_debug_section (separate_debug_str, i->handle))
	continue;

      section = &debug_displays [separate_debug_str].section;

      if (section->start == NULL)
	continue;

      if (offset >= section->size)
	continue;

      ret = (const char *) (section->start + offset);
      /* Unfortunately we cannot rely upon the .debug_str section ending with a
	 NUL byte.  Since our caller is expecting to receive a well formed C
	 string we test for the lack of a terminating byte here.  */
      if (strnlen ((const char *) ret, section->size - offset)
	  == section->size - offset)
	return _("<no NUL byte at end of alt .debug_str section>");

      return ret;
    }

  warn (_("DW_FORM_GNU_strp_alt offset (%s) too big or no string sections available\n"),
	dwarf_vmatoa ("x", offset));
  return _("<offset is too big>");
}