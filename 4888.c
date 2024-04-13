check_for_and_load_links (void * file, const char * filename)
{
  void * handle = NULL;

  if (load_debug_section (gnu_debugaltlink, file))
    {
      Build_id_data build_id_data;

      handle = load_separate_debug_info (filename,
					 & debug_displays[gnu_debugaltlink].section,
					 parse_gnu_debugaltlink,
					 check_gnu_debugaltlink,
					 & build_id_data,
					 file);
      if (handle)
	{
	  assert (handle == first_separate_info->handle);
	  check_for_and_load_links (first_separate_info->handle,
				    first_separate_info->filename);
	}
    }

  if (load_debug_section (gnu_debuglink, file))
    {
      unsigned long crc32;

      handle = load_separate_debug_info (filename,
					 & debug_displays[gnu_debuglink].section,
					 parse_gnu_debuglink,
					 check_gnu_debuglink,
					 & crc32,
					 file);
      if (handle)
	{
	  assert (handle == first_separate_info->handle);
	  check_for_and_load_links (first_separate_info->handle,
				    first_separate_info->filename);
	}
    }

  load_debug_sup_file (filename, file);

  load_build_id_debug_file (filename, file);
}