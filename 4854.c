display_debug_ranges (struct dwarf_section *section,
		      void *file ATTRIBUTE_UNUSED)
{
  unsigned char *       start = section->start;
  unsigned char *       last_start = start;
  dwarf_vma             bytes = section->size;
  unsigned char *       section_begin = start;
  unsigned char *       finish = start + bytes;
  unsigned int          num_range_list, i;
  struct range_entry *  range_entries;
  struct range_entry *  range_entry_fill;
  int                   is_rnglists = strstr (section->name, "debug_rnglists") != NULL;
  /* Initialize it due to a false compiler warning.  */
  unsigned char         address_size = 0;
  dwarf_vma             last_offset = 0;
  unsigned int          offset_size = 0;

  if (bytes == 0)
    {
      printf (_("\nThe %s section is empty.\n"), section->name);
      return 0;
    }

  introduce (section, false);

  if (is_rnglists)
    {
      dwarf_vma       initial_length;
      unsigned char   segment_selector_size;
      unsigned int    offset_entry_count;
      unsigned short  version;

      /* Get and check the length of the block.  */
      SAFE_BYTE_GET_AND_INC (initial_length, start, 4, finish);

      if (initial_length == 0xffffffff)
	{
	  /* This section is 64-bit DWARF 3.  */
	  SAFE_BYTE_GET_AND_INC (initial_length, start, 8, finish);
	  offset_size = 8;
	}
      else
	offset_size = 4;

      if (initial_length > (size_t) (finish - start))
	{
	  /* If the length field has a relocation against it, then we should
	     not complain if it is inaccurate (and probably negative).
	     It is copied from .debug_line handling code.  */
	  if (reloc_at (section, (start - section->start) - offset_size))
	    {
	      initial_length = finish - start;
	    }
	  else
	    {
	      warn (_("The length field (0x%lx) in the debug_rnglists header is wrong - the section is too small\n"),
		    (long) initial_length);
	      return 0;
	    }
	}

      /* Get the other fields in the header.  */
      SAFE_BYTE_GET_AND_INC (version, start, 2, finish);
      SAFE_BYTE_GET_AND_INC (address_size, start, 1, finish);
      SAFE_BYTE_GET_AND_INC (segment_selector_size, start, 1, finish);
      SAFE_BYTE_GET_AND_INC (offset_entry_count, start, 4, finish);

      printf (_("  Length:          0x%s\n"), dwarf_vmatoa ("x", initial_length));
      printf (_("  DWARF version:   %u\n"), version);
      printf (_("  Address size:    %u\n"), address_size);
      printf (_("  Segment size:    %u\n"), segment_selector_size);
      printf (_("  Offset entries:  %u\n"), offset_entry_count);

      /* Check the fields.  */
      if (segment_selector_size != 0)
	{
	  warn (_("The %s section contains "
		  "unsupported segment selector size: %d.\n"),
		section->name, segment_selector_size);
	  return 0;
	}

      if (version < 5)
	{
	  warn (_("Only DWARF version 5+ debug_rnglists info "
		  "is currently supported.\n"));
	  return 0;
	}

      if (offset_entry_count != 0)
	{
	  printf (_("\n   Offsets starting at 0x%lx:\n"), (long)(start - section->start));
	  if (offset_size == 8)
	    {
	      for (i = 0; i < offset_entry_count; i++)
		{
		  dwarf_vma entry;

		  SAFE_BYTE_GET_AND_INC (entry, start, 8, finish);
		  printf (_("    [%6u] 0x%s\n"), i, dwarf_vmatoa ("x", entry));
		}
	    }
	  else
	    {
	      for (i = 0; i < offset_entry_count; i++)
		{
		  uint32_t entry;

		  SAFE_BYTE_GET_AND_INC (entry, start, 4, finish);
		  printf (_("    [%6u] 0x%x\n"), i, entry);
		}
	    }
	}
    }
  
  if (load_debug_info (file) == 0)
    {
      warn (_("Unable to load/parse the .debug_info section, so cannot interpret the %s section.\n"),
	    section->name);
      return 0;
    }

  num_range_list = 0;
  for (i = 0; i < num_debug_info_entries; i++)
    {
      if (debug_information [i].dwarf_version < 5 && is_rnglists)
	/* Skip .debug_rnglists reference.  */
	continue;
      if (debug_information [i].dwarf_version >= 5 && !is_rnglists)
	/* Skip .debug_range reference.  */
	continue;
      num_range_list += debug_information [i].num_range_lists;
    }

  if (num_range_list == 0)
    {
      /* This can happen when the file was compiled with -gsplit-debug
	 which removes references to range lists from the primary .o file.  */
      printf (_("No range lists in .debug_info section.\n"));
      return 1;
    }

  range_entries = (struct range_entry *)
      xmalloc (sizeof (*range_entries) * num_range_list);
  range_entry_fill = range_entries;

  for (i = 0; i < num_debug_info_entries; i++)
    {
      debug_info *debug_info_p = &debug_information[i];
      unsigned int j;

      if (debug_information [i].dwarf_version < 5 && is_rnglists)
	/* Skip .debug_rnglists reference.  */
	continue;
      if (debug_information [i].dwarf_version >= 5 && !is_rnglists)
	/* Skip .debug_range reference.  */
	continue;

      for (j = 0; j < debug_info_p->num_range_lists; j++)
	{
	  range_entry_fill->ranges_offset = debug_info_p->range_lists[j];
	  range_entry_fill->debug_info_p = debug_info_p;
	  range_entry_fill++;
	}
    }

  qsort (range_entries, num_range_list, sizeof (*range_entries),
	 range_entry_compar);

  if (dwarf_check != 0 && range_entries[0].ranges_offset != 0)
    warn (_("Range lists in %s section start at 0x%lx\n"),
	  section->name, (unsigned long) range_entries[0].ranges_offset);

  putchar ('\n');
  printf (_("    Offset   Begin    End\n"));

  for (i = 0; i < num_range_list; i++)
    {
      struct range_entry *range_entry = &range_entries[i];
      debug_info *debug_info_p = range_entry->debug_info_p;
      unsigned int pointer_size;
      dwarf_vma offset;
      unsigned char *next;
      dwarf_vma base_address;

      pointer_size = (is_rnglists ? address_size : debug_info_p->pointer_size);
      offset = range_entry->ranges_offset;
      base_address = debug_info_p->base_address;

      /* PR 17512: file: 001-101485-0.001:0.1.  */
      if (pointer_size < 2 || pointer_size > 8)
	{
	  warn (_("Corrupt pointer size (%d) in debug entry at offset %8.8lx\n"),
		pointer_size, (unsigned long) offset);
	  continue;
	}

      if (offset > (size_t) (finish - section_begin))
	{
	  warn (_("Corrupt offset (%#8.8lx) in range entry %u\n"),
		(unsigned long) offset, i);
	  continue;
	}

      next = section_begin + offset + debug_info_p->rnglists_base;

      /* If multiple DWARF entities reference the same range then we will
         have multiple entries in the `range_entries' list for the same
         offset.  Thanks to the sort above these will all be consecutive in
         the `range_entries' list, so we can easily ignore duplicates
         here.  */
      if (i > 0 && last_offset == offset)
        continue;
      last_offset = offset;

      if (dwarf_check != 0 && i > 0)
	{
	  if (start < next)
	    warn (_("There is a hole [0x%lx - 0x%lx] in %s section.\n"),
		  (unsigned long) (start - section_begin),
		  (unsigned long) (next - section_begin), section->name);
	  else if (start > next)
	    {
	      if (next == last_start)
		continue;
	      warn (_("There is an overlap [0x%lx - 0x%lx] in %s section.\n"),
		    (unsigned long) (start - section_begin),
		    (unsigned long) (next - section_begin), section->name);
	    }
	}

      start = next;
      last_start = next;

      if (is_rnglists)
	display_debug_rnglists_list
	  (start, finish, pointer_size, offset, base_address, offset_size);
      else
	display_debug_ranges_list
	  (start, finish, pointer_size, offset, base_address);
    }
  putchar ('\n');

  free (range_entries);

  return 1;
}