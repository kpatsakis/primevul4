process_abbrev_set (struct dwarf_section *section,
		    dwarf_vma abbrev_base,
		    dwarf_vma abbrev_size,
		    dwarf_vma abbrev_offset,
		    abbrev_list *list)
{
  if (abbrev_base >= section->size
      || abbrev_size > section->size - abbrev_base)
    {
      /* PR 17531: file:4bcd9ce9.  */
      warn (_("Debug info is corrupted, abbrev size (%lx) is larger than "
	      "abbrev section size (%lx)\n"),
	      (unsigned long) (abbrev_base + abbrev_size),
	      (unsigned long) section->size);
      return NULL;
    }
  if (abbrev_offset >= abbrev_size)
    {
      warn (_("Debug info is corrupted, abbrev offset (%lx) is larger than "
	      "abbrev section size (%lx)\n"),
	    (unsigned long) abbrev_offset,
	    (unsigned long) abbrev_size);
      return NULL;
    }

  unsigned char *start = section->start + abbrev_base;
  unsigned char *end = start + abbrev_size;
  start += abbrev_offset;
  while (start < end)
    {
      unsigned long entry;
      unsigned long tag;
      unsigned long attribute;
      int children;

      READ_ULEB (entry, start, end);

      /* A single zero is supposed to end the set according
	 to the standard.  If there's more, then signal that to
	 the caller.  */
      if (start == end)
	return NULL;
      if (entry == 0)
	return start;

      READ_ULEB (tag, start, end);
      if (start == end)
	return NULL;

      children = *start++;

      add_abbrev (entry, tag, children, list);

      do
	{
	  unsigned long form;
	  /* Initialize it due to a false compiler warning.  */
	  dwarf_signed_vma implicit_const = -1;

	  READ_ULEB (attribute, start, end);
	  if (start == end)
	    break;

	  READ_ULEB (form, start, end);
	  if (start == end)
	    break;

	  if (form == DW_FORM_implicit_const)
	    {
	      READ_SLEB (implicit_const, start, end);
	      if (start == end)
		break;
	    }

	  add_abbrev_attr (attribute, form, implicit_const, list);
	}
      while (attribute != 0);
    }

  /* Report the missing single zero which ends the section.  */
  error (_(".debug_abbrev section not zero terminated\n"));

  return NULL;
}