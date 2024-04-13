display_debug_addr (struct dwarf_section *section,
		    void *file)
{
  debug_info **debug_addr_info;
  unsigned char *entry;
  unsigned char *end;
  unsigned int i;
  unsigned int count;
  unsigned char * header;

  if (section->size == 0)
    {
      printf (_("\nThe %s section is empty.\n"), section->name);
      return 0;
    }

  if (load_debug_info (file) == 0)
    {
      warn (_("Unable to load/parse the .debug_info section, so cannot interpret the %s section.\n"),
	    section->name);
      return 0;
    }

  introduce (section, false);

  /* PR  17531: file: cf38d01b.
     We use xcalloc because a corrupt file may not have initialised all of the
     fields in the debug_info structure, which means that the sort below might
     try to move uninitialised data.  */
  debug_addr_info = (debug_info **) xcalloc ((num_debug_info_entries + 1),
					     sizeof (debug_info *));

  count = 0;
  for (i = 0; i < num_debug_info_entries; i++)
    if (debug_information [i].addr_base != DEBUG_INFO_UNAVAILABLE)
      {
	/* PR 17531: file: cf38d01b.  */
	if (debug_information[i].addr_base >= section->size)
	  warn (_("Corrupt address base (%lx) found in debug section %u\n"),
		(unsigned long) debug_information[i].addr_base, i);
	else
	  debug_addr_info [count++] = debug_information + i;
      }

  /* Add a sentinel to make iteration convenient.  */
  debug_addr_info [count] = (debug_info *) xmalloc (sizeof (debug_info));
  debug_addr_info [count]->addr_base = section->size;
  qsort (debug_addr_info, count, sizeof (debug_info *), comp_addr_base);

  header = section->start;
  for (i = 0; i < count; i++)
    {
      unsigned int idx;
      unsigned int address_size = debug_addr_info [i]->pointer_size;

      printf (_("  For compilation unit at offset 0x%s:\n"),
	      dwarf_vmatoa ("x", debug_addr_info [i]->cu_offset));

      printf (_("\tIndex\tAddress\n"));
      entry = section->start + debug_addr_info [i]->addr_base;
      if (debug_addr_info [i]->dwarf_version >= 5)
	{
	  size_t           header_size = entry - header;
	  unsigned char *  curr_header = header;
	  dwarf_vma        length;
	  int              version;
	  int              segment_selector_size;

	  if (header_size != 8 && header_size != 16)
	    {
	      warn (_("Corrupt %s section: expecting header size of 8 or 16, but found %ld instead\n"),
		    section->name, (long) header_size);
	      return 0;
	    }

	  SAFE_BYTE_GET_AND_INC (length, curr_header, 4, entry);
	  if (length == 0xffffffff)
	    SAFE_BYTE_GET_AND_INC (length, curr_header, 8, entry);
	  end = curr_header + length;

	  SAFE_BYTE_GET_AND_INC (version, curr_header, 2, entry);
	  if (version != 5)
	    warn (_("Corrupt %s section: expecting version number 5 in header but found %d instead\n"),
		  section->name, version);

	  SAFE_BYTE_GET_AND_INC (address_size, curr_header, 1, entry);
	  SAFE_BYTE_GET_AND_INC (segment_selector_size, curr_header, 1, entry);
	  address_size += segment_selector_size;
	}
      else
	end = section->start + debug_addr_info [i + 1]->addr_base;
      header = end;
      idx = 0;
      while (entry < end)
	{
	  dwarf_vma base = byte_get (entry, address_size);
	  printf (_("\t%d:\t"), idx);
	  print_dwarf_vma (base, address_size);
	  printf ("\n");
	  entry += address_size;
	  idx++;
	}
    }
  printf ("\n");

  free (debug_addr_info);
  return 1;
}