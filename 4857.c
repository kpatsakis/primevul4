display_debug_ranges_list (unsigned char *  start,
			   unsigned char *  finish,
			   unsigned int     pointer_size,
			   dwarf_vma        offset,
			   dwarf_vma        base_address)
{
  while (start < finish)
    {
      dwarf_vma begin;
      dwarf_vma end;

      SAFE_BYTE_GET_AND_INC (begin, start, pointer_size, finish);
      if (start >= finish)
	break;
      SAFE_SIGNED_BYTE_GET_AND_INC (end, start, pointer_size, finish);

      printf ("    ");
      print_dwarf_vma (offset, 4);

      if (begin == 0 && end == 0)
	{
	  printf (_("<End of list>\n"));
	  break;
	}

      /* Check base address specifiers.  */
      if (is_max_address (begin, pointer_size)
	  && !is_max_address (end, pointer_size))
	{
	  base_address = end;
	  print_dwarf_vma (begin, pointer_size);
	  print_dwarf_vma (end, pointer_size);
	  printf ("(base address)\n");
	  continue;
	}

      print_dwarf_vma (begin + base_address, pointer_size);
      print_dwarf_vma (end + base_address, pointer_size);

      if (begin == end)
	fputs (_("(start == end)"), stdout);
      else if (begin > end)
	fputs (_("(start > end)"), stdout);

      putchar ('\n');
    }
}