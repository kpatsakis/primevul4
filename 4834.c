display_loclists_list (struct dwarf_section *  section,
		       unsigned char **        start_ptr,
		       unsigned int            debug_info_entry,
		       dwarf_vma               offset,
		       dwarf_vma               base_address,
		       unsigned char **        vstart_ptr,
		       int                     has_frame_base)
{
  unsigned char *  start = *start_ptr;
  unsigned char *  vstart = *vstart_ptr;
  unsigned char *  section_end = section->start + section->size;
  dwarf_vma        cu_offset;
  unsigned int     pointer_size;
  unsigned int     offset_size;
  unsigned int     dwarf_version;

  /* Initialize it due to a false compiler warning.  */
  dwarf_vma begin = -1, vbegin = -1;
  dwarf_vma end = -1, vend = -1;
  dwarf_vma length;
  int need_frame_base;

  if (debug_info_entry >= num_debug_info_entries)
    {
      warn (_("No debug information available for "
	      "loclists lists of entry: %u\n"),
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
      dwarf_vma off = offset + (start - *start_ptr);
      enum dwarf_location_list_entry_type llet;

      if (start + 1 > section_end)
	{
	  warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		(unsigned long) offset);
	  break;
	}

      printf ("    ");
      print_dwarf_vma (off, 4);

      SAFE_BYTE_GET_AND_INC (llet, start, 1, section_end);

      if (vstart && (llet == DW_LLE_offset_pair
		     || llet == DW_LLE_start_end
		     || llet == DW_LLE_start_length))
	{
	  off = offset + (vstart - *start_ptr);

	  READ_ULEB (vbegin, vstart, section_end);
	  print_dwarf_view (vbegin, pointer_size, 1);

	  READ_ULEB (vend, vstart, section_end);
	  print_dwarf_view (vend, pointer_size, 1);

	  printf (_("views at %8.8lx for:\n    %*s "),
		  (unsigned long) off, 8, "");
	}

      switch (llet)
	{
	case DW_LLE_end_of_list:
	  printf (_("<End of list>\n"));
	  break;

	case DW_LLE_base_addressx:
	  READ_ULEB (base_address, start, section_end);
	  print_dwarf_vma (base_address, pointer_size);
	  printf (_("(index into .debug_addr) "));
	  base_address = fetch_indexed_addr (base_address, pointer_size);
	  print_dwarf_vma (base_address, pointer_size);
	  printf (_("(base address)\n"));
	  break;

	case DW_LLE_startx_endx:
	  READ_ULEB (begin, start, section_end);
	  begin = fetch_indexed_addr (begin, pointer_size);
	  READ_ULEB (end, start, section_end);
	  end = fetch_indexed_addr (end, pointer_size);
	  break;

	case DW_LLE_startx_length:
	  READ_ULEB (begin, start, section_end);
	  begin = fetch_indexed_addr (begin, pointer_size);
	  READ_ULEB (end, start, section_end);
	  end += begin;
	  break;

	case DW_LLE_default_location:
	  begin = end = 0;
	  break;
	  
	case DW_LLE_offset_pair:
	  READ_ULEB (begin, start, section_end);
	  begin += base_address;
	  READ_ULEB (end, start, section_end);
	  end += base_address;
	  break;

	case DW_LLE_base_address:
	  SAFE_BYTE_GET_AND_INC (base_address, start, pointer_size,
				 section_end);
	  print_dwarf_vma (base_address, pointer_size);
	  printf (_("(base address)\n"));
	  break;

	case DW_LLE_start_end:
	  SAFE_BYTE_GET_AND_INC (begin, start, pointer_size, section_end);
	  SAFE_BYTE_GET_AND_INC (end, start, pointer_size, section_end);
	  break;

	case DW_LLE_start_length:
	  SAFE_BYTE_GET_AND_INC (begin, start, pointer_size, section_end);
	  READ_ULEB (end, start, section_end);
	  end += begin;
	  break;

#ifdef DW_LLE_view_pair
	case DW_LLE_view_pair:
	  if (vstart)
	    printf (_("View pair entry in loclist with locviews attribute\n"));
	  READ_ULEB (vbegin, start, section_end);
	  print_dwarf_view (vbegin, pointer_size, 1);

	  READ_ULEB (vend, start, section_end);
	  print_dwarf_view (vend, pointer_size, 1);

	  printf (_("views for:\n"));
	  continue;
#endif

	default:
	  error (_("Invalid location list entry type %d\n"), llet);
	  return;
	}

      if (llet == DW_LLE_end_of_list)
	break;

      if (llet == DW_LLE_base_address
	  || llet == DW_LLE_base_addressx)
	continue;

      if (start == section_end)
	{
	  warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		(unsigned long) offset);
	  break;
	}
      READ_ULEB (length, start, section_end);

      if (length > (size_t) (section_end - start))
	{
	  warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		(unsigned long) offset);
	  break;
	}

      print_dwarf_vma (begin, pointer_size);
      print_dwarf_vma (end, pointer_size);

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

      if (begin == end && vbegin == vend)
	fputs (_(" (start == end)"), stdout);
      else if (begin > end || (begin == end && vbegin > vend))
	fputs (_(" (start > end)"), stdout);

      putchar ('\n');

      start += length;
      vbegin = vend = -1;
    }

  if (vbegin != vm1 || vend != vm1)
    printf (_("Trailing view pair not used in a range"));

  *start_ptr = start;
  *vstart_ptr = vstart;
}