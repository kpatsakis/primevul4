display_offset_entry_loclists (struct dwarf_section *section)
{
  unsigned char *  start = section->start;
  unsigned char * const end = start + section->size;

  introduce (section, false);  

  do
    {
      dwarf_vma        length;
      unsigned short   version;
      unsigned char    address_size;
      unsigned char    segment_selector_size;
      uint32_t         offset_entry_count;
      uint32_t         i;
      bool             is_64bit;

      printf (_("Table at Offset 0x%lx\n"), (long)(start - section->start));

      SAFE_BYTE_GET_AND_INC (length, start, 4, end);
      if (length == 0xffffffff)
	{
	  is_64bit = true;
	  SAFE_BYTE_GET_AND_INC (length, start, 8, end);
	}
      else
	is_64bit = false;

      SAFE_BYTE_GET_AND_INC (version, start, 2, end);
      SAFE_BYTE_GET_AND_INC (address_size, start, 1, end);
      SAFE_BYTE_GET_AND_INC (segment_selector_size, start, 1, end);
      SAFE_BYTE_GET_AND_INC (offset_entry_count, start, 4, end);

      printf (_("  Length:          0x%s\n"), dwarf_vmatoa ("x", length));
      printf (_("  DWARF version:   %u\n"), version);
      printf (_("  Address size:    %u\n"), address_size);
      printf (_("  Segment size:    %u\n"), segment_selector_size);
      printf (_("  Offset entries:  %u\n"), offset_entry_count);

      if (version < 5)
	{
	  warn (_("The %s section contains a corrupt or "
		  "unsupported version number: %d.\n"),
		section->name, version);
	  return 0;
	}

      if (segment_selector_size != 0)
	{
	  warn (_("The %s section contains an "
		  "unsupported segment selector size: %d.\n"),
		section->name, segment_selector_size);
	  return 0;
	}
      
      if (offset_entry_count == 0)
	{
	  warn (_("The %s section contains a table without offset\n"),
		section->name);
	  return 0;
	}
  
      printf (_("\n   Offset Entries starting at 0x%lx:\n"),
	      (long)(start - section->start));

      if (is_64bit)
	{
	  for (i = 0; i < offset_entry_count; i++)
	    {
	      dwarf_vma entry;

	      SAFE_BYTE_GET_AND_INC (entry, start, 8, end);
	      printf (_("    [%6u] 0x%s\n"), i, dwarf_vmatoa ("x", entry));
	    }
	}
      else
	{
	  for (i = 0; i < offset_entry_count; i++)
	    {
	      uint32_t entry;

	      SAFE_BYTE_GET_AND_INC (entry, start, 4, end);
	      printf (_("    [%6u] 0x%x\n"), i, entry);
	    }
	}

      putchar ('\n');

      uint32_t j;

      for (j = 1, i = 0; i < offset_entry_count;)
	{
	  unsigned char  lle;
	  dwarf_vma      base_address = 0;
	  dwarf_vma      begin;
	  dwarf_vma      finish;
	  dwarf_vma      off = start - section->start;

	  if (j != i)
	    {
	      printf (_("   Offset Entry %u\n"), i);
	      j = i;
	    }

	  printf ("    ");
	  print_dwarf_vma (off, 4);

	  SAFE_BYTE_GET_AND_INC (lle, start, 1, end);

	  switch (lle)
	    {
	    case DW_LLE_end_of_list:
	      printf (_("<End of list>\n\n"));
	      i ++;
	      continue;

	    case DW_LLE_base_addressx:
	      READ_ULEB (base_address, start, end);
	      print_dwarf_vma (base_address, address_size);
	      printf (_("(index into .debug_addr) "));
	      base_address = fetch_indexed_addr (base_address, address_size);
	      print_dwarf_vma (base_address, address_size);
	      printf (_("(base address)\n"));
	      continue;

	    case DW_LLE_startx_endx:
	      READ_ULEB (begin, start, end);
	      begin = fetch_indexed_addr (begin, address_size);
	      READ_ULEB (finish, start, end);
	      finish = fetch_indexed_addr (finish, address_size);
	      break;

	    case DW_LLE_startx_length:
	      READ_ULEB (begin, start, end);
	      begin = fetch_indexed_addr (begin, address_size);
	      READ_ULEB (finish, start, end);
	      finish += begin;
	      break;

	    case DW_LLE_offset_pair:
	      READ_ULEB (begin, start, end);
	      begin += base_address;
	      READ_ULEB (finish, start, end);
	      finish += base_address;
	      break;

	    case DW_LLE_default_location:
	      begin = finish = 0;
	      break;

	    case DW_LLE_base_address:
	      SAFE_BYTE_GET_AND_INC (base_address, start, address_size, end);
	      print_dwarf_vma (base_address, address_size);
	      printf (_("(base address)\n"));
	      continue;

	    case DW_LLE_start_end:
	      SAFE_BYTE_GET_AND_INC (begin,  start, address_size, end);
	      SAFE_BYTE_GET_AND_INC (finish, start, address_size, end);
	      break;

	    case DW_LLE_start_length:
	      SAFE_BYTE_GET_AND_INC (begin, start, address_size, end);
	      READ_ULEB (finish, start, end);
	      finish += begin;
	      break;

	    default:
	      error (_("Invalid location list entry type %d\n"), lle);
	      return 0;
	    }

	  if (start == end)
	    {
	      warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		    (unsigned long) off);
	      break;
	    }

	  print_dwarf_vma (begin, address_size);
	  print_dwarf_vma (finish, address_size);

	  if (begin == finish)
	    fputs (_(" (start == end)"), stdout);
	  else if (begin > finish)
	    fputs (_(" (start > end)"), stdout);

	  /* Read the counted location descriptions.  */
	  READ_ULEB (length, start, end);

	  if (length > (size_t) (end - start))
	    {
	      warn (_("Location list starting at offset 0x%lx is not terminated.\n"),
		    (unsigned long) off);
	      break;
	    }

	  putchar (' ');
	  (void) decode_location_expression (start, address_size, address_size,
					     version, length, 0, section);
	  start += length;
	  putchar ('\n');
	}

      putchar ('\n');
    }
  while (start < end);

  return 1;
}