process_debug_info (struct dwarf_section * section,
		    void *file,
		    enum dwarf_section_display_enum abbrev_sec,
		    bool do_loc,
		    bool do_types)
{
  unsigned char *start = section->start;
  unsigned char *end = start + section->size;
  unsigned char *section_begin;
  unsigned int unit;
  unsigned int num_units = 0;

  /* First scan the section to get the number of comp units.
     Length sanity checks are done here.  */
  for (section_begin = start, num_units = 0; section_begin < end;
       num_units ++)
    {
      dwarf_vma length;

      /* Read the first 4 bytes.  For a 32-bit DWARF section, this
	 will be the length.  For a 64-bit DWARF section, it'll be
	 the escape code 0xffffffff followed by an 8 byte length.  */
      SAFE_BYTE_GET_AND_INC (length, section_begin, 4, end);

      if (length == 0xffffffff)
	SAFE_BYTE_GET_AND_INC (length, section_begin, 8, end);
      else if (length >= 0xfffffff0 && length < 0xffffffff)
	{
	  warn (_("Reserved length value (0x%s) found in section %s\n"),
		dwarf_vmatoa ("x", length), section->name);
	  return false;
	}

      /* Negative values are illegal, they may even cause infinite
	 looping.  This can happen if we can't accurately apply
	 relocations to an object file, or if the file is corrupt.  */
      if (length > (size_t) (end - section_begin))
	{
	  warn (_("Corrupt unit length (got 0x%s expected at most 0x%s) in section %s\n"),
		dwarf_vmatoa ("x", length),
		dwarf_vmatoa ("x", end - section_begin),
		section->name);
	  return false;
	}
      section_begin += length;
    }

  if (num_units == 0)
    {
      error (_("No comp units in %s section ?\n"), section->name);
      return false;
    }

  if ((do_loc || do_debug_loc || do_debug_ranges || do_debug_info)
      && num_debug_info_entries == 0
      && ! do_types)
    {

      /* Then allocate an array to hold the information.  */
      debug_information = (debug_info *) cmalloc (num_units,
						  sizeof (* debug_information));
      if (debug_information == NULL)
	{
	  error (_("Not enough memory for a debug info array of %u entries\n"),
		 num_units);
	  alloc_num_debug_info_entries = num_debug_info_entries = 0;
	  return false;
	}

      /* PR 17531: file: 92ca3797.
	 We cannot rely upon the debug_information array being initialised
	 before it is used.  A corrupt file could easily contain references
	 to a unit for which information has not been made available.  So
	 we ensure that the array is zeroed here.  */
      memset (debug_information, 0, num_units * sizeof (*debug_information));

      alloc_num_debug_info_entries = num_units;
    }

  if (!do_loc)
    {
      load_debug_section_with_follow (str, file);
      load_debug_section_with_follow (line_str, file);
      load_debug_section_with_follow (str_dwo, file);
      load_debug_section_with_follow (str_index, file);
      load_debug_section_with_follow (str_index_dwo, file);
      load_debug_section_with_follow (debug_addr, file);
    }

  load_debug_section_with_follow (abbrev_sec, file);
  load_debug_section_with_follow (loclists, file);
  load_debug_section_with_follow (rnglists, file);
  
  if (debug_displays [abbrev_sec].section.start == NULL)
    {
      warn (_("Unable to locate %s section!\n"),
	    debug_displays [abbrev_sec].section.uncompressed_name);
      return false;
    }

  if (!do_loc && dwarf_start_die == 0)
    introduce (section, false);

  free_all_abbrevs ();
  free (cu_abbrev_map);
  cu_abbrev_map = NULL;
  next_free_abbrev_map_entry = 0;

  /* In order to be able to resolve DW_FORM_ref_attr forms we need
     to load *all* of the abbrevs for all CUs in this .debug_info
     section.  This does effectively mean that we (partially) read
     every CU header twice.  */
  for (section_begin = start; start < end;)
    {
      DWARF2_Internal_CompUnit  compunit;
      unsigned char *           hdrptr;
      dwarf_vma                 abbrev_base;
      size_t                    abbrev_size;
      dwarf_vma                 cu_offset;
      unsigned int              offset_size;
      struct cu_tu_set *        this_set;
      abbrev_list *             list;
      unsigned char *end_cu;

      hdrptr = start;
      cu_offset = start - section_begin;

      SAFE_BYTE_GET_AND_INC (compunit.cu_length, hdrptr, 4, end);

      if (compunit.cu_length == 0xffffffff)
	{
	  SAFE_BYTE_GET_AND_INC (compunit.cu_length, hdrptr, 8, end);
	  offset_size = 8;
	}
      else
	offset_size = 4;
      end_cu = hdrptr + compunit.cu_length;

      SAFE_BYTE_GET_AND_INC (compunit.cu_version, hdrptr, 2, end_cu);

      this_set = find_cu_tu_set_v2 (cu_offset, do_types);

      if (compunit.cu_version < 5)
	{
	  compunit.cu_unit_type = DW_UT_compile;
	  /* Initialize it due to a false compiler warning.  */
	  compunit.cu_pointer_size = -1;
	}
      else
	{
	  SAFE_BYTE_GET_AND_INC (compunit.cu_unit_type, hdrptr, 1, end_cu);
	  do_types = (compunit.cu_unit_type == DW_UT_type);

	  SAFE_BYTE_GET_AND_INC (compunit.cu_pointer_size, hdrptr, 1, end_cu);
	}

      SAFE_BYTE_GET_AND_INC (compunit.cu_abbrev_offset, hdrptr, offset_size,
			     end_cu);

      if (compunit.cu_unit_type == DW_UT_split_compile
	  || compunit.cu_unit_type == DW_UT_skeleton)
	{
	  uint64_t dwo_id;
	  SAFE_BYTE_GET_AND_INC (dwo_id, hdrptr, 8, end_cu);
	}

      if (this_set == NULL)
	{
	  abbrev_base = 0;
	  abbrev_size = debug_displays [abbrev_sec].section.size;
	}
      else
	{
	  abbrev_base = this_set->section_offsets [DW_SECT_ABBREV];
	  abbrev_size = this_set->section_sizes [DW_SECT_ABBREV];
	}

      list = find_abbrev_list_by_abbrev_offset (abbrev_base,
						compunit.cu_abbrev_offset);
      if (list == NULL)
	{
	  unsigned char *  next;

	  list = new_abbrev_list (abbrev_base,
				  compunit.cu_abbrev_offset);
	  next = process_abbrev_set (&debug_displays[abbrev_sec].section,
				     abbrev_base, abbrev_size,
				     compunit.cu_abbrev_offset, list);
	  list->start_of_next_abbrevs = next;
	}

      start = end_cu;
      record_abbrev_list_for_cu (cu_offset, start - section_begin, list);
    }

  for (start = section_begin, unit = 0; start < end; unit++)
    {
      DWARF2_Internal_CompUnit compunit;
      unsigned char *hdrptr;
      unsigned char *tags;
      int level, last_level, saved_level;
      dwarf_vma cu_offset;
      unsigned int offset_size;
      dwarf_vma signature = 0;
      dwarf_vma type_offset = 0;
      struct cu_tu_set *this_set;
      dwarf_vma abbrev_base;
      size_t abbrev_size;
      abbrev_list * list = NULL;
      unsigned char *end_cu;

      hdrptr = start;
      cu_offset = start - section_begin;

      SAFE_BYTE_GET_AND_INC (compunit.cu_length, hdrptr, 4, end);

      if (compunit.cu_length == 0xffffffff)
	{
	  SAFE_BYTE_GET_AND_INC (compunit.cu_length, hdrptr, 8, end);
	  offset_size = 8;
	}
      else
	offset_size = 4;
      end_cu = hdrptr + compunit.cu_length;

      SAFE_BYTE_GET_AND_INC (compunit.cu_version, hdrptr, 2, end_cu);

      this_set = find_cu_tu_set_v2 (cu_offset, do_types);

      if (compunit.cu_version < 5)
	{
	  compunit.cu_unit_type = DW_UT_compile;
	  /* Initialize it due to a false compiler warning.  */
	  compunit.cu_pointer_size = -1;
	}
      else
	{
	  SAFE_BYTE_GET_AND_INC (compunit.cu_unit_type, hdrptr, 1, end_cu);
	  do_types = (compunit.cu_unit_type == DW_UT_type);

	  SAFE_BYTE_GET_AND_INC (compunit.cu_pointer_size, hdrptr, 1, end_cu);
	}

      SAFE_BYTE_GET_AND_INC (compunit.cu_abbrev_offset, hdrptr, offset_size, end_cu);

      if (this_set == NULL)
	{
	  abbrev_base = 0;
	  abbrev_size = debug_displays [abbrev_sec].section.size;
	}
      else
	{
	  abbrev_base = this_set->section_offsets [DW_SECT_ABBREV];
	  abbrev_size = this_set->section_sizes [DW_SECT_ABBREV];
	}

      if (compunit.cu_version < 5)
	SAFE_BYTE_GET_AND_INC (compunit.cu_pointer_size, hdrptr, 1, end_cu);

      bool do_dwo_id = false;
      uint64_t dwo_id = 0;
      if (compunit.cu_unit_type == DW_UT_split_compile
	  || compunit.cu_unit_type == DW_UT_skeleton)
	{
	  SAFE_BYTE_GET_AND_INC (dwo_id, hdrptr, 8, end_cu);
	  do_dwo_id = true;
	}

      /* PR 17512: file: 001-108546-0.001:0.1.  */
      if (compunit.cu_pointer_size < 2 || compunit.cu_pointer_size > 8)
	{
	  warn (_("Invalid pointer size (%d) in compunit header, using %d instead\n"),
		compunit.cu_pointer_size, offset_size);
	  compunit.cu_pointer_size = offset_size;
	}

      if (do_types)
	{
	  SAFE_BYTE_GET_AND_INC (signature, hdrptr, 8, end_cu);
	  SAFE_BYTE_GET_AND_INC (type_offset, hdrptr, offset_size, end_cu);
	}

      if (dwarf_start_die >= (size_t) (end_cu - section_begin))
	{
	  start = end_cu;
	  continue;
	}

      if ((do_loc || do_debug_loc || do_debug_ranges || do_debug_info)
	  && num_debug_info_entries == 0
	  && alloc_num_debug_info_entries > unit
	  && ! do_types)
	{
	  debug_information [unit].cu_offset = cu_offset;
	  debug_information [unit].pointer_size
	    = compunit.cu_pointer_size;
	  debug_information [unit].offset_size = offset_size;
	  debug_information [unit].dwarf_version = compunit.cu_version;
	  debug_information [unit].base_address = 0;
	  debug_information [unit].addr_base = DEBUG_INFO_UNAVAILABLE;
	  debug_information [unit].ranges_base = DEBUG_INFO_UNAVAILABLE;
	  debug_information [unit].loc_offsets = NULL;
	  debug_information [unit].have_frame_base = NULL;
	  debug_information [unit].max_loc_offsets = 0;
	  debug_information [unit].num_loc_offsets = 0;
	  debug_information [unit].loclists_base = 0;
	  debug_information [unit].range_lists = NULL;
	  debug_information [unit].max_range_lists= 0;
	  debug_information [unit].num_range_lists = 0;
	  debug_information [unit].rnglists_base = 0;
	  debug_information [unit].str_offsets_base = 0;
	}

      if (!do_loc && dwarf_start_die == 0)
	{
	  printf (_("  Compilation Unit @ offset 0x%s:\n"),
		  dwarf_vmatoa ("x", cu_offset));
	  printf (_("   Length:        0x%s (%s)\n"),
		  dwarf_vmatoa ("x", compunit.cu_length),
		  offset_size == 8 ? "64-bit" : "32-bit");
	  printf (_("   Version:       %d\n"), compunit.cu_version);
	  if (compunit.cu_version >= 5)
	    {
	      const char *name = get_DW_UT_name (compunit.cu_unit_type);

	      printf (_("   Unit Type:     %s (%x)\n"),
		      name ? name : "???",
		      compunit.cu_unit_type);
	    }
	  printf (_("   Abbrev Offset: 0x%s\n"),
		  dwarf_vmatoa ("x", compunit.cu_abbrev_offset));
	  printf (_("   Pointer Size:  %d\n"), compunit.cu_pointer_size);
	  if (do_types)
	    {
	      printf (_("   Signature:     0x%s\n"),
		      dwarf_vmatoa ("x", signature));
	      printf (_("   Type Offset:   0x%s\n"),
		      dwarf_vmatoa ("x", type_offset));
	    }
	  if (do_dwo_id)
	    printf (_("   DWO ID:        0x%s\n"), dwarf_vmatoa ("x", dwo_id));
	  if (this_set != NULL)
	    {
	      dwarf_vma *offsets = this_set->section_offsets;
	      size_t *sizes = this_set->section_sizes;

	      printf (_("   Section contributions:\n"));
	      printf (_("    .debug_abbrev.dwo:       0x%s  0x%s\n"),
		      dwarf_vmatoa ("x", offsets [DW_SECT_ABBREV]),
		      dwarf_vmatoa ("x", sizes [DW_SECT_ABBREV]));
	      printf (_("    .debug_line.dwo:         0x%s  0x%s\n"),
		      dwarf_vmatoa ("x", offsets [DW_SECT_LINE]),
		      dwarf_vmatoa ("x", sizes [DW_SECT_LINE]));
	      printf (_("    .debug_loc.dwo:          0x%s  0x%s\n"),
		      dwarf_vmatoa ("x", offsets [DW_SECT_LOC]),
		      dwarf_vmatoa ("x", sizes [DW_SECT_LOC]));
	      printf (_("    .debug_str_offsets.dwo:  0x%s  0x%s\n"),
		      dwarf_vmatoa ("x", offsets [DW_SECT_STR_OFFSETS]),
		      dwarf_vmatoa ("x", sizes [DW_SECT_STR_OFFSETS]));
	    }
	}

      tags = hdrptr;
      start = end_cu;

      if (compunit.cu_version < 2 || compunit.cu_version > 5)
	{
	  warn (_("CU at offset %s contains corrupt or "
		  "unsupported version number: %d.\n"),
		dwarf_vmatoa ("x", cu_offset), compunit.cu_version);
	  continue;
	}

      if (compunit.cu_unit_type != DW_UT_compile
	  && compunit.cu_unit_type != DW_UT_partial
	  && compunit.cu_unit_type != DW_UT_type
	  && compunit.cu_unit_type != DW_UT_split_compile
	  && compunit.cu_unit_type != DW_UT_skeleton)
	{
	  warn (_("CU at offset %s contains corrupt or "
		  "unsupported unit type: %d.\n"),
		dwarf_vmatoa ("x", cu_offset), compunit.cu_unit_type);
	  continue;
	}

      /* Process the abbrevs used by this compilation unit.  */
      list = find_abbrev_list_by_abbrev_offset (abbrev_base,
						compunit.cu_abbrev_offset);
      if (list == NULL)
	{
	  unsigned char *next;

	  list = new_abbrev_list (abbrev_base,
				  compunit.cu_abbrev_offset);
	  next = process_abbrev_set (&debug_displays[abbrev_sec].section,
				     abbrev_base, abbrev_size,
				     compunit.cu_abbrev_offset, list);
	  list->start_of_next_abbrevs = next;
	}

      level = 0;
      last_level = level;
      saved_level = -1;
      while (tags < start)
	{
	  unsigned long abbrev_number;
	  unsigned long die_offset;
	  abbrev_entry *entry;
	  abbrev_attr *attr;
	  int do_printing = 1;

	  die_offset = tags - section_begin;

	  READ_ULEB (abbrev_number, tags, start);

	  /* A null DIE marks the end of a list of siblings or it may also be
	     a section padding.  */
	  if (abbrev_number == 0)
	    {
	      /* Check if it can be a section padding for the last CU.  */
	      if (level == 0 && start == end)
		{
		  unsigned char *chk;

		  for (chk = tags; chk < start; chk++)
		    if (*chk != 0)
		      break;
		  if (chk == start)
		    break;
		}

	      if (!do_loc && die_offset >= dwarf_start_die
		  && (dwarf_cutoff_level == -1
		      || level < dwarf_cutoff_level))
		printf (_(" <%d><%lx>: Abbrev Number: 0\n"),
			level, die_offset);

	      --level;
	      if (level < 0)
		{
		  static unsigned num_bogus_warns = 0;

		  if (num_bogus_warns < 3)
		    {
		      warn (_("Bogus end-of-siblings marker detected at offset %lx in %s section\n"),
			    die_offset, section->name);
		      num_bogus_warns ++;
		      if (num_bogus_warns == 3)
			warn (_("Further warnings about bogus end-of-sibling markers suppressed\n"));
		    }
		}
	      if (dwarf_start_die != 0 && level < saved_level)
		return true;
	      continue;
	    }

	  if (!do_loc)
	    {
	      if (dwarf_start_die != 0 && die_offset < dwarf_start_die)
		do_printing = 0;
	      else
		{
		  if (dwarf_start_die != 0 && die_offset == dwarf_start_die)
		    saved_level = level;
		  do_printing = (dwarf_cutoff_level == -1
				 || level < dwarf_cutoff_level);
		  if (do_printing)
		    printf (_(" <%d><%lx>: Abbrev Number: %lu"),
			    level, die_offset, abbrev_number);
		  else if (dwarf_cutoff_level == -1
			   || last_level < dwarf_cutoff_level)
		    printf (_(" <%d><%lx>: ...\n"), level, die_offset);
		  last_level = level;
		}
	    }

	  /* Scan through the abbreviation list until we reach the
	     correct entry.  */
	  if (list == NULL)
	    continue;

	  for (entry = list->first_abbrev; entry != NULL; entry = entry->next)
	    if (entry->number == abbrev_number)
	      break;

	  if (entry == NULL)
	    {
	      if (!do_loc && do_printing)
		{
		  printf ("\n");
		  fflush (stdout);
		}
	      warn (_("DIE at offset 0x%lx refers to abbreviation number %lu which does not exist\n"),
		    die_offset, abbrev_number);
	      return false;
	    }

	  if (!do_loc && do_printing)
	    printf (" (%s)\n", get_TAG_name (entry->tag));

	  switch (entry->tag)
	    {
	    default:
	      need_base_address = 0;
	      break;
	    case DW_TAG_compile_unit:
	      need_base_address = 1;	
	      need_dwo_info = do_loc;
	      break;
	    case DW_TAG_entry_point:
	    case DW_TAG_subprogram:
	      need_base_address = 0;
	      /* Assuming that there is no DW_AT_frame_base.  */
	      have_frame_base = 0;
	      break;
	    }

	  debug_info *debug_info_p =
	    (debug_information && unit < alloc_num_debug_info_entries)
	    ? debug_information + unit : NULL;

	  assert (!debug_info_p
		  || (debug_info_p->num_loc_offsets
		      == debug_info_p->num_loc_views));

	  for (attr = entry->first_attr;
	       attr && attr->attribute;
	       attr = attr->next)
	    {
	      if (! do_loc && do_printing)
		/* Show the offset from where the tag was extracted.  */
		printf ("    <%lx>", (unsigned long)(tags - section_begin));
	      tags = read_and_display_attr (attr->attribute,
					    attr->form,
					    attr->implicit_const,
					    section_begin,
					    tags,
					    start,
					    cu_offset,
					    compunit.cu_pointer_size,
					    offset_size,
					    compunit.cu_version,
					    debug_info_p,
					    do_loc || ! do_printing,
					    section,
					    this_set,
					    level);
	    }

	  /* If a locview attribute appears before a location one,
	     make sure we don't associate it with an earlier
	     loclist. */
	  if (debug_info_p)
	    switch (debug_info_p->num_loc_offsets - debug_info_p->num_loc_views)
	      {
	      case 1:
		debug_info_p->loc_views [debug_info_p->num_loc_views] = vm1;
		debug_info_p->num_loc_views++;
		assert (debug_info_p->num_loc_views
			== debug_info_p->num_loc_offsets);
		break;

	      case 0:
		break;

	      case -1:
		warn(_("DIE has locviews without loclist\n"));
		debug_info_p->num_loc_views--;
		break;

	      default:
		assert (0);
	    }

	  if (entry->children)
	    ++level;
	}
    }

  /* Set num_debug_info_entries here so that it can be used to check if
     we need to process .debug_loc and .debug_ranges sections.  */
  if ((do_loc || do_debug_loc || do_debug_ranges || do_debug_info)
      && num_debug_info_entries == 0
      && ! do_types)
    {
      if (num_units > alloc_num_debug_info_entries)
	num_debug_info_entries = alloc_num_debug_info_entries;
      else
	num_debug_info_entries = num_units;
    }

  if (!do_loc)
    printf ("\n");

  return true;
}