free_debug_memory (void)
{
  unsigned int i;

  free_all_abbrevs ();

  free (cu_abbrev_map);
  cu_abbrev_map = NULL;
  next_free_abbrev_map_entry = 0;

  free (shndx_pool);
  shndx_pool = NULL;
  shndx_pool_size = 0;
  shndx_pool_used = 0;
  free (cu_sets);
  cu_sets = NULL;
  cu_count = 0;
  free (tu_sets);
  tu_sets = NULL;
  tu_count = 0;

  memset (level_type_signed, 0, sizeof level_type_signed);
  cu_tu_indexes_read = -1;

  for (i = 0; i < max; i++)
    free_debug_section ((enum dwarf_section_display_enum) i);

  if (debug_information != NULL)
    {
      for (i = 0; i < alloc_num_debug_info_entries; i++)
	{
	  if (debug_information [i].max_loc_offsets)
	    {
	      free (debug_information [i].loc_offsets);
	      free (debug_information [i].have_frame_base);
	    }
	  if (debug_information [i].max_range_lists)
	    free (debug_information [i].range_lists);
	}
      free (debug_information);
      debug_information = NULL;
      alloc_num_debug_info_entries = num_debug_info_entries = 0;
    }

  separate_info * d;
  separate_info * next;

  for (d = first_separate_info; d != NULL; d = next)
    {
      close_debug_file (d->handle);
      free ((void *) d->filename);
      next = d->next;
      free ((void *) d);
    }
  first_separate_info = NULL;

  free_dwo_info ();
}