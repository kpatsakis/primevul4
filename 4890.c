display_debug_pubnames_worker (struct dwarf_section *section,
			       void *file ATTRIBUTE_UNUSED,
			       int is_gnu)
{
  DWARF2_Internal_PubNames names;
  unsigned char *start = section->start;
  unsigned char *end = start + section->size;

  /* It does not matter if this load fails,
     we test for that later on.  */
  load_debug_info (file);

  introduce (section, false);

  while (start < end)
    {
      unsigned char *data;
      unsigned long sec_off = start - section->start;
      unsigned int offset_size;

      SAFE_BYTE_GET_AND_INC (names.pn_length, start, 4, end);
      if (names.pn_length == 0xffffffff)
	{
	  SAFE_BYTE_GET_AND_INC (names.pn_length, start, 8, end);
	  offset_size = 8;
	}
      else
	offset_size = 4;

      if (names.pn_length > (size_t) (end - start))
	{
	  warn (_("Debug info is corrupted, %s header at %#lx has length %s\n"),
		section->name,
		sec_off,
		dwarf_vmatoa ("x", names.pn_length));
	  break;
	}

      data = start;
      start += names.pn_length;

      SAFE_BYTE_GET_AND_INC (names.pn_version, data, 2, start);
      SAFE_BYTE_GET_AND_INC (names.pn_offset, data, offset_size, start);

      if (num_debug_info_entries != DEBUG_INFO_UNAVAILABLE
	  && num_debug_info_entries > 0
	  && find_debug_info_for_offset (names.pn_offset) == NULL)
	warn (_(".debug_info offset of 0x%lx in %s section does not point to a CU header.\n"),
	      (unsigned long) names.pn_offset, section->name);

      SAFE_BYTE_GET_AND_INC (names.pn_size, data, offset_size, start);

      printf (_("  Length:                              %ld\n"),
	      (long) names.pn_length);
      printf (_("  Version:                             %d\n"),
	      names.pn_version);
      printf (_("  Offset into .debug_info section:     0x%lx\n"),
	      (unsigned long) names.pn_offset);
      printf (_("  Size of area in .debug_info section: %ld\n"),
	      (long) names.pn_size);

      if (names.pn_version != 2 && names.pn_version != 3)
	{
	  static int warned = 0;

	  if (! warned)
	    {
	      warn (_("Only DWARF 2 and 3 pubnames are currently supported\n"));
	      warned = 1;
	    }

	  continue;
	}

      if (is_gnu)
	printf (_("\n    Offset  Kind          Name\n"));
      else
	printf (_("\n    Offset\tName\n"));

      while (1)
	{
	  bfd_size_type maxprint;
	  dwarf_vma offset;

	  SAFE_BYTE_GET_AND_INC (offset, data, offset_size, start);

	  if (offset == 0)
	    break;

	  if (data >= start)
	    break;
	  maxprint = (start - data) - 1;

	  if (is_gnu)
	    {
	      unsigned int kind_data;
	      gdb_index_symbol_kind kind;
	      const char *kind_name;
	      int is_static;

	      SAFE_BYTE_GET_AND_INC (kind_data, data, 1, start);
	      maxprint --;
	      /* GCC computes the kind as the upper byte in the CU index
		 word, and then right shifts it by the CU index size.
		 Left shift KIND to where the gdb-index.h accessor macros
		 can use it.  */
	      kind_data <<= GDB_INDEX_CU_BITSIZE;
	      kind = GDB_INDEX_SYMBOL_KIND_VALUE (kind_data);
	      kind_name = get_gdb_index_symbol_kind_name (kind);
	      is_static = GDB_INDEX_SYMBOL_STATIC_VALUE (kind_data);
	      printf ("    %-6lx  %s,%-10s  %.*s\n",
		      (unsigned long) offset, is_static ? _("s") : _("g"),
		      kind_name, (int) maxprint, data);
	    }
	  else
	    printf ("    %-6lx\t%.*s\n",
		    (unsigned long) offset, (int) maxprint, data);

	  data += strnlen ((char *) data, maxprint);
	  if (data < start)
	    data++;
	  if (data >= start)
	    break;
	}
    }

  printf ("\n");
  return 1;
}