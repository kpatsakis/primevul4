display_loc_list (struct dwarf_section *section,
		  unsigned char **start_ptr,
		  unsigned int debug_info_entry,
		  dwarf_vma offset,
		  dwarf_vma base_address,
		  unsigned char **vstart_ptr,
		  int has_frame_base)
{
  unsigned char *start = *start_ptr, *vstart = *vstart_ptr;
  unsigned char *section_end = section->start + section->size;
  dwarf_vma    cu_offset;
  unsigned int pointer_size;
  unsigned int offset_size;
  int dwarf_version;
  dwarf_vma begin;
  dwarf_vma end;
  unsigned short length;
  int need_frame_base;

  if (debug_info_entry >= num_debug_info_entries)
    {
      warn (_("No debug information available for loc lists of entry: %u\n"),
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
      dwarf_vma vbegin = vm1, vend = vm1;

      if (2 * pointer_size > (size_t) (section_end - start))
	{
	  warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		(unsigned long) offset);
	  break;
	}

      printf ("    ");
      print_dwarf_vma (off, 4);

      SAFE_BYTE_GET_AND_INC (begin, start, pointer_size, section_end);
      SAFE_BYTE_GET_AND_INC (end, start, pointer_size, section_end);

      if (begin == 0 && end == 0)
	{
	  /* PR 18374: In a object file we can have a location list that
	     starts with a begin and end of 0 because there are relocations
	     that need to be applied to the addresses.  Actually applying
	     the relocations now does not help as they will probably resolve
	     to 0, since the object file has not been fully linked.  Real
	     end of list markers will not have any relocations against them.  */
	  if (! reloc_at (section, off)
	      && ! reloc_at (section, off + pointer_size))
	    {
	      printf (_("<End of list>\n"));
	      break;
	    }
	}

      /* Check base address specifiers.  */
      if (is_max_address (begin, pointer_size)
          && !is_max_address (end, pointer_size))
	{
	  base_address = end;
	  print_dwarf_vma (begin, pointer_size);
	  print_dwarf_vma (end, pointer_size);
	  printf (_("(base address)\n"));
	  continue;
	}

      if (vstart)
	{
	  off = offset + (vstart - *start_ptr);

	  READ_ULEB (vbegin, vstart, section_end);
	  print_dwarf_view (vbegin, pointer_size, 1);

	  READ_ULEB (vend, vstart, section_end);
	  print_dwarf_view (vend, pointer_size, 1);

	  printf (_("views at %8.8lx for:\n    %*s "),
		  (unsigned long) off, 8, "");
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

      print_dwarf_vma (begin + base_address, pointer_size);
      print_dwarf_vma (end + base_address, pointer_size);

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
    }

  *start_ptr = start;
  *vstart_ptr = vstart;
}