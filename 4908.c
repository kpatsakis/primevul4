display_loc_list_dwo (struct dwarf_section *section,
		      unsigned char **start_ptr,
		      unsigned int debug_info_entry,
		      dwarf_vma offset,
		      unsigned char **vstart_ptr,
		      int has_frame_base)
{
  unsigned char *start = *start_ptr, *vstart = *vstart_ptr;
  unsigned char *section_end = section->start + section->size;
  dwarf_vma    cu_offset;
  unsigned int pointer_size;
  unsigned int offset_size;
  int dwarf_version;
  int entry_type;
  unsigned short length;
  int need_frame_base;
  unsigned int idx;

  if (debug_info_entry >= num_debug_info_entries)
    {
      warn (_("No debug information for loc lists of entry: %u\n"),
	    debug_info_entry);
      return;
    }

  cu_offset = debug_information [debug_info_entry].cu_offset;
  pointer_size = debug_information [debug_info_entry].pointer_size;
  offset_size = debug_information [debug_info_entry].offset_size;
  dwarf_version = debug_information [debug_info_entry].dwarf_version;

  if (pointer_size < 2 || pointer_size > 8)
    {
      warn (_("Invalid pointer size (%d) in debug info for entry %d\n"),
	    pointer_size, debug_info_entry);
      return;
    }

  while (1)
    {
      printf ("    ");
      print_dwarf_vma (offset + (start - *start_ptr), 4);

      if (start >= section_end)
	{
	  warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		(unsigned long) offset);
	  break;
	}

      SAFE_BYTE_GET_AND_INC (entry_type, start, 1, section_end);

      if (vstart)
	switch (entry_type)
	  {
	  default:
	    break;

	  case 2:
	  case 3:
	  case 4:
	    {
	      dwarf_vma view;
	      dwarf_vma off = offset + (vstart - *start_ptr);

	      READ_ULEB (view, vstart, section_end);
	      print_dwarf_view (view, 8, 1);

	      READ_ULEB (view, vstart, section_end);
	      print_dwarf_view (view, 8, 1);

	      printf (_("views at %8.8lx for:\n    %*s "),
		      (unsigned long) off, 8, "");

	    }
	    break;
	  }

      switch (entry_type)
	{
	case 0: /* A terminating entry.  */
	  *start_ptr = start;
	  *vstart_ptr = vstart;
	  printf (_("<End of list>\n"));
	  return;
	case 1: /* A base-address entry.  */
	  READ_ULEB (idx, start, section_end);
	  print_addr_index (idx, 8);
	  printf ("%*s", 9 + (vstart ? 2 * 6 : 0), "");
	  printf (_("(base address selection entry)\n"));
	  continue;
	case 2: /* A start/end entry.  */
	  READ_ULEB (idx, start, section_end);
	  print_addr_index (idx, 8);
	  READ_ULEB (idx, start, section_end);
	  print_addr_index (idx, 8);
	  break;
	case 3: /* A start/length entry.  */
	  READ_ULEB (idx, start, section_end);
	  print_addr_index (idx, 8);
	  SAFE_BYTE_GET_AND_INC (idx, start, 4, section_end);
	  printf ("%08x ", idx);
	  break;
	case 4: /* An offset pair entry.  */
	  SAFE_BYTE_GET_AND_INC (idx, start, 4, section_end);
	  printf ("%08x ", idx);
	  SAFE_BYTE_GET_AND_INC (idx, start, 4, section_end);
	  printf ("%08x ", idx);
	  break;
	default:
	  warn (_("Unknown location list entry type 0x%x.\n"), entry_type);
	  *start_ptr = start;
	  *vstart_ptr = vstart;
	  return;
	}

      if (2 > (size_t) (section_end - start))
	{
	  warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		(unsigned long) offset);
	  break;
	}

      SAFE_BYTE_GET_AND_INC (length, start, 2, section_end);
      if (length > (size_t) (section_end - start))
	{
	  warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		(unsigned long) offset);
	  break;
	}

      putchar ('(');
      need_frame_base = decode_location_expression (start,
						    pointer_size,
						    offset_size,
						    dwarf_version,
						    length,
						    cu_offset, section);
      putchar (')');

      if (need_frame_base && !has_frame_base)
	printf (_(" [without DW_AT_frame_base]"));

      putchar ('\n');

      start += length;
    }

  *start_ptr = start;
  *vstart_ptr = vstart;
}