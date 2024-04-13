display_debug_loc (struct dwarf_section *section, void *file)
{
  unsigned char *start = section->start, *vstart = NULL;
  dwarf_vma bytes;
  unsigned char *section_begin = start;
  unsigned int num_loc_list = 0;
  dwarf_vma last_offset = 0;
  dwarf_vma last_view = 0;
  unsigned int first = 0;
  unsigned int i;
  unsigned int j;
  int seen_first_offset = 0;
  int locs_sorted = 1;
  unsigned char *next = start, *vnext = vstart;
  unsigned int *array = NULL;
  const char *suffix = strrchr (section->name, '.');
  bool is_dwo = false;
  int is_loclists = strstr (section->name, "debug_loclists") != NULL;
  dwarf_vma expected_start = 0;

  if (suffix && strcmp (suffix, ".dwo") == 0)
    is_dwo = true;

  bytes = section->size;

  if (bytes == 0)
    {
      printf (_("\nThe %s section is empty.\n"), section->name);
      return 0;
    }

  if (is_loclists)
    {
      unsigned char *hdrptr = section_begin;
      dwarf_vma ll_length;
      unsigned short ll_version;
      unsigned char *end = section_begin + section->size;
      unsigned char address_size, segment_selector_size;
      uint32_t offset_entry_count;

      SAFE_BYTE_GET_AND_INC (ll_length, hdrptr, 4, end);
      if (ll_length == 0xffffffff)
	SAFE_BYTE_GET_AND_INC (ll_length, hdrptr, 8, end);

      SAFE_BYTE_GET_AND_INC (ll_version, hdrptr, 2, end);
      if (ll_version != 5)
	{
	  warn (_("The %s section contains corrupt or "
		  "unsupported version number: %d.\n"),
		section->name, ll_version);
	  return 0;
	}

      SAFE_BYTE_GET_AND_INC (address_size, hdrptr, 1, end);

      SAFE_BYTE_GET_AND_INC (segment_selector_size, hdrptr, 1, end);
      if (segment_selector_size != 0)
	{
	  warn (_("The %s section contains "
		  "unsupported segment selector size: %d.\n"),
		section->name, segment_selector_size);
	  return 0;
	}

      SAFE_BYTE_GET_AND_INC (offset_entry_count, hdrptr, 4, end);

      if (offset_entry_count != 0)
	return display_offset_entry_loclists (section);

      expected_start = hdrptr - section_begin;
    }

  if (load_debug_info (file) == 0)
    {
      warn (_("Unable to load/parse the .debug_info section, so cannot interpret the %s section.\n"),
	    section->name);
      return 0;
    }

  /* Check the order of location list in .debug_info section. If
     offsets of location lists are in the ascending order, we can
     use `debug_information' directly.  */
  for (i = 0; i < num_debug_info_entries; i++)
    {
      unsigned int num;

      num = debug_information [i].num_loc_offsets;
      if (num > num_loc_list)
	num_loc_list = num;

      /* Check if we can use `debug_information' directly.  */
      if (locs_sorted && num != 0)
	{
	  if (!seen_first_offset)
	    {
	      /* This is the first location list.  */
	      last_offset = debug_information [i].loc_offsets [0];
	      last_view = debug_information [i].loc_views [0];
	      first = i;
	      seen_first_offset = 1;
	      j = 1;
	    }
	  else
	    j = 0;

	  for (; j < num; j++)
	    {
	      if (last_offset >
		  debug_information [i].loc_offsets [j]
		  || (last_offset == debug_information [i].loc_offsets [j]
		      && last_view > debug_information [i].loc_views [j]))
		{
		  locs_sorted = 0;
		  break;
		}
	      last_offset = debug_information [i].loc_offsets [j];
	      last_view = debug_information [i].loc_views [j];
	    }
	}
    }

  if (!seen_first_offset)
    error (_("No location lists in .debug_info section!\n"));

  if (debug_information [first].num_loc_offsets > 0
      && debug_information [first].loc_offsets [0] != expected_start
      && debug_information [first].loc_views [0] != expected_start)
    warn (_("Location lists in %s section start at 0x%s rather than 0x%s\n"),
	  section->name,
	  dwarf_vmatoa ("x", debug_information [first].loc_offsets [0]),
	  dwarf_vmatoa ("x", expected_start));

  if (!locs_sorted)
    array = (unsigned int *) xcmalloc (num_loc_list, sizeof (unsigned int));

  introduce (section, false);

  if (reloc_at (section, 0))
    printf (_(" Warning: This section has relocations - addresses seen here may not be accurate.\n\n"));

  printf (_("    Offset   Begin            End              Expression\n"));

  seen_first_offset = 0;
  for (i = first; i < num_debug_info_entries; i++)
    {
      dwarf_vma offset, voffset;
      dwarf_vma base_address;
      unsigned int k;
      int has_frame_base;

      if (!locs_sorted)
	{
	  for (k = 0; k < debug_information [i].num_loc_offsets; k++)
	    array[k] = k;
	  loc_offsets = debug_information [i].loc_offsets;
	  loc_views = debug_information [i].loc_views;
	  qsort (array, debug_information [i].num_loc_offsets,
		 sizeof (*array), loc_offsets_compar);
	}

      int adjacent_view_loclists = 1;
      for (k = 0; k < debug_information [i].num_loc_offsets; k++)
	{
	  j = locs_sorted ? k : array[k];
	  if (k
	      && (debug_information [i].loc_offsets [locs_sorted
						    ? k - 1 : array [k - 1]]
		  == debug_information [i].loc_offsets [j])
	      && (debug_information [i].loc_views [locs_sorted
						   ? k - 1 : array [k - 1]]
		  == debug_information [i].loc_views [j]))
	    continue;
	  has_frame_base = debug_information [i].have_frame_base [j];
	  offset = debug_information [i].loc_offsets [j];
	  next = section_begin + offset;
	  voffset = debug_information [i].loc_views [j];
	  if (voffset != vm1)
	    vnext = section_begin + voffset;
	  else
	    vnext = NULL;
	  base_address = debug_information [i].base_address;

	  if (vnext && vnext < next)
	    {
	      vstart = vnext;
	      display_view_pair_list (section, &vstart, i, next);
	      if (start == vnext)
		start = vstart;
	    }

	  if (!seen_first_offset || !adjacent_view_loclists)
	    seen_first_offset = 1;
	  else
	    {
	      if (start < next)
		warn (_("There is a hole [0x%lx - 0x%lx] in %s section.\n"),
		      (unsigned long) (start - section_begin),
		      (unsigned long) offset,
		      section->name);
	      else if (start > next)
		warn (_("There is an overlap [0x%lx - 0x%lx] in %s section.\n"),
		      (unsigned long) (start - section_begin),
		      (unsigned long) offset,
		      section->name);
	    }
	  start = next;
	  vstart = vnext;

	  if (offset >= bytes)
	    {
	      warn (_("Offset 0x%lx is bigger than %s section size.\n"),
		    (unsigned long) offset,
		    section->name);
	      continue;
	    }

	  if (vnext && voffset >= bytes)
	    {
	      warn (_("View Offset 0x%lx is bigger than %s section size.\n"),
		    (unsigned long) voffset,
		    section->name);
	      continue;
	    }

	  if (!is_loclists)
	    {
	      if (is_dwo)
		display_loc_list_dwo (section, &start, i, offset,
				      &vstart, has_frame_base);
	      else
		display_loc_list (section, &start, i, offset, base_address,
				  &vstart, has_frame_base);
	    }
	  else
	    {
	      if (is_dwo)
		warn (_("DWO is not yet supported.\n"));
	      else
		display_loclists_list (section, &start, i, offset, base_address,
				       &vstart, has_frame_base);
	    }

	  /* FIXME: this arrangement is quite simplistic.  Nothing
	     requires locview lists to be adjacent to corresponding
	     loclists, and a single loclist could be augmented by
	     different locview lists, and vice-versa, unlikely as it
	     is that it would make sense to do so.  Hopefully we'll
	     have view pair support built into loclists before we ever
	     need to address all these possibilities.  */
	  if (adjacent_view_loclists && vnext
	      && vnext != start && vstart != next)
	    {
	      adjacent_view_loclists = 0;
	      warn (_("Hole and overlap detection requires adjacent view lists and loclists.\n"));
	    }

	  if (vnext && vnext == start)
	    display_view_pair_list (section, &start, i, vstart);
	}
    }

  if (start < section->start + section->size)
    warn (ngettext ("There is %ld unused byte at the end of section %s\n",
		    "There are %ld unused bytes at the end of section %s\n",
		    (long) (section->start + section->size - start)),
	  (long) (section->start + section->size - start), section->name);
  putchar ('\n');
  free (array);
  return 1;
}