display_debug_rnglists_list (unsigned char * start,
			     unsigned char * finish,
			     unsigned int    pointer_size,
			     dwarf_vma       offset,
			     dwarf_vma       base_address,
			     unsigned int    offset_size)
{
  unsigned char *next = start;
  unsigned int debug_addr_section_hdr_len;

  if (offset_size == 4)
    debug_addr_section_hdr_len = 8;
  else
    debug_addr_section_hdr_len = 16;

  while (1)
    {
      dwarf_vma off = offset + (start - next);
      enum dwarf_range_list_entry rlet;
      /* Initialize it due to a false compiler warning.  */
      dwarf_vma begin = -1, length, end = -1;

      if (start >= finish)
	{
	  warn (_("Range list starting at offset 0x%s is not terminated.\n"),
		dwarf_vmatoa ("x", offset));
	  break;
	}

      printf ("    ");
      print_dwarf_vma (off, 4);

      SAFE_BYTE_GET_AND_INC (rlet, start, 1, finish);

      switch (rlet)
	{
	case DW_RLE_end_of_list:
	  printf (_("<End of list>\n"));
	  break;
	case DW_RLE_base_addressx:
	  READ_ULEB (base_address, start, finish);
	  print_dwarf_vma (base_address, pointer_size);
	  printf (_("(base address index) "));
	  base_address = fetch_indexed_addr ((base_address * pointer_size)
			                     + debug_addr_section_hdr_len, pointer_size);
	  print_dwarf_vma (base_address, pointer_size);
	  printf (_("(base address)\n"));
	  break;
	case DW_RLE_startx_endx:
	  READ_ULEB (begin, start, finish);
	  READ_ULEB (end, start, finish);
	  begin = fetch_indexed_addr ((begin * pointer_size)
			              + debug_addr_section_hdr_len, pointer_size);
	  end   = fetch_indexed_addr ((begin * pointer_size)
			              + debug_addr_section_hdr_len, pointer_size);
	  break;
	case DW_RLE_startx_length:
	  READ_ULEB (begin, start, finish);
	  READ_ULEB (length, start, finish);
	  begin = fetch_indexed_addr ((begin * pointer_size)
			              + debug_addr_section_hdr_len, pointer_size);
	  end = begin + length;
	  break;
	case DW_RLE_offset_pair:
	  READ_ULEB (begin, start, finish);
	  READ_ULEB (end, start, finish);
	  break;
	case DW_RLE_base_address:
	  SAFE_BYTE_GET_AND_INC (base_address, start, pointer_size, finish);
	  print_dwarf_vma (base_address, pointer_size);
	  printf (_("(base address)\n"));
	  break;
	case DW_RLE_start_end:
	  SAFE_BYTE_GET_AND_INC (begin, start, pointer_size, finish);
	  SAFE_BYTE_GET_AND_INC (end, start, pointer_size, finish);
	  break;
	case DW_RLE_start_length:
	  SAFE_BYTE_GET_AND_INC (begin, start, pointer_size, finish);
	  READ_ULEB (length, start, finish);
	  end = begin + length;
	  break;
	default:
	  error (_("Invalid range list entry type %d\n"), rlet);
	  rlet = DW_RLE_end_of_list;
	  break;
	}

      if (rlet == DW_RLE_end_of_list)
	break;
      if (rlet == DW_RLE_base_address || rlet == DW_RLE_base_addressx)
	continue;

      /* Only a DW_RLE_offset_pair needs the base address added.  */
      if (rlet == DW_RLE_offset_pair)
	{
	  begin += base_address;
	  end += base_address;
	}

      print_dwarf_vma (begin, pointer_size);
      print_dwarf_vma (end, pointer_size);

      if (begin == end)
	fputs (_("(start == end)"), stdout);
      else if (begin > end)
	fputs (_("(start > end)"), stdout);

      putchar ('\n');
    }
}