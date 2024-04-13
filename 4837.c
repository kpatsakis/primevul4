display_debug_str_offsets (struct dwarf_section *section,
			   void *file ATTRIBUTE_UNUSED)
{
  unsigned long idx;

  if (section->size == 0)
    {
      printf (_("\nThe %s section is empty.\n"), section->name);
      return 0;
    }

  unsigned char *start = section->start;
  unsigned char *end = start + section->size;
  unsigned char *curr = start;

  const char *suffix = strrchr (section->name, '.');
  bool dwo = suffix && strcmp (suffix, ".dwo") == 0;

  if (dwo)
    load_debug_section_with_follow (str_dwo, file);
  else
    load_debug_section_with_follow (str, file);

  introduce (section, false);

  while (curr < end)
    {
      dwarf_vma length;
      dwarf_vma entry_length;

      SAFE_BYTE_GET_AND_INC (length, curr, 4, end);
      /* FIXME: We assume that this means 64-bit DWARF is being used.  */
      if (length == 0xffffffff)
	{
	  SAFE_BYTE_GET_AND_INC (length, curr, 8, end);
	  entry_length = 8;
	}
      else
	entry_length = 4;

      unsigned char *entries_end;
      if (length == 0)
	{
	  /* This is probably an old style .debug_str_offset section which
	     just contains offsets and no header (and the first offset is 0).  */
	  length = section->size;
	  curr   = section->start;
	  entries_end = end;

	  printf (_("    Length: %#lx\n"), (unsigned long) length);
	  printf (_("       Index   Offset [String]\n"));
	}
      else
	{
	  if (length <= (dwarf_vma) (end - curr))
	    entries_end = curr + length;
	  else
	    {
	      warn (_("Section %s is too small %#lx\n"),
		    section->name, (unsigned long) section->size);
	      entries_end = end;
	    }

	  int version;
	  SAFE_BYTE_GET_AND_INC (version, curr, 2, entries_end);
	  if (version != 5)
	    warn (_("Unexpected version number in str_offset header: %#x\n"), version);

	  int padding;
	  SAFE_BYTE_GET_AND_INC (padding, curr, 2, entries_end);
	  if (padding != 0)
	    warn (_("Unexpected value in str_offset header's padding field: %#x\n"), padding);

	  printf (_("    Length: %#lx\n"), (unsigned long) length);
	  printf (_("    Version: %#lx\n"), (unsigned long) version);
	  printf (_("       Index   Offset [String]\n"));
	}

      for (idx = 0; curr < entries_end; idx++)
	{
	  dwarf_vma offset;
	  const unsigned char * string;

	  if ((dwarf_vma) (entries_end - curr) < entry_length)
	    /* Not enough space to read one entry_length, give up.  */
	    return 0;

	  SAFE_BYTE_GET_AND_INC (offset, curr, entry_length, entries_end);
	  if (dwo)
	    string = (const unsigned char *)
	      fetch_indexed_string (idx, NULL, entry_length, dwo, 0);
	  else
	    string = fetch_indirect_string (offset);

	  printf ("    %8lu %8s %s\n", idx, dwarf_vmatoa ("x", offset),
		  string);
	}
    }

  return 1;
}