load_separate_debug_files (void * file, const char * filename)
{
  /* Skip this operation if we are not interested in debug links.  */
  if (! do_follow_links && ! do_debug_links)
    return false;

  /* See if there are any dwo links.  */
  if (load_debug_section (str, file)
      && load_debug_section (abbrev, file)
      && load_debug_section (info, file))
    {
      /* Load the .debug_addr section, if it exists.  */
      load_debug_section (debug_addr, file);
      /* Load the .debug_str_offsets section, if it exists.  */
      load_debug_section (str_index, file);

      free_dwo_info ();

      if (process_debug_info (& debug_displays[info].section, file, abbrev,
			      true, false))
	{
	  bool introduced = false;
	  dwo_info *dwinfo;
	  const char *dir = NULL;
	  const char *id = NULL;
	  const char *name = NULL;

	  for (dwinfo = first_dwo_info; dwinfo != NULL; dwinfo = dwinfo->next)
	    {
	      /* Accumulate NAME, DIR and ID fields.  */
	      switch (dwinfo->type)
		{
		case DWO_NAME:
		  if (name != NULL)
		    warn (_("Multiple DWO_NAMEs encountered for the same CU\n"));
		  name = dwinfo->value;
		  break;

		case DWO_DIR:
		  /* There can be multiple DW_AT_comp_dir entries in a CU,
		     so do not complain.  */
		  dir = dwinfo->value;
		  break;

		case DWO_ID:
		  if (id != NULL)
		    warn (_("multiple DWO_IDs encountered for the same CU\n"));
		  id = dwinfo->value;
		  break;

		default:
		  error (_("Unexpected DWO INFO type"));
		  break;
		}

	      /* If we have reached the end of our list, or we are changing
		 CUs, then display the information that we have accumulated
		 so far.  */
	      if (name != NULL
		  && (dwinfo->next == NULL
		      || dwinfo->next->cu_offset != dwinfo->cu_offset))
		{
		  if (do_debug_links)
		    {
		      if (! introduced)
			{
			  printf (_("The %s section contains link(s) to dwo file(s):\n\n"),
				  debug_displays [info].section.uncompressed_name);
			  introduced = true;
			}

		      printf (_("  Name:      %s\n"), name);
		      printf (_("  Directory: %s\n"), dir ? dir : _("<not-found>"));
		      if (id != NULL)
			display_data (printf (_("  ID:       ")), (unsigned char *) id, 8);
		      else
			printf (_("  ID:        <not specified>\n"));
		      printf ("\n\n");
		    }

		  if (do_follow_links)
		    load_dwo_file (filename, name, dir, id);

		  name = dir = id = NULL;
		}
	    }
	}
    }

  if (! do_follow_links)
    /* The other debug links will be displayed by display_debug_links()
       so we do not need to do any further processing here.  */
    return false;

  /* FIXME: We do not check for the presence of both link sections in the same file.  */
  /* FIXME: We do not check for the presence of multiple, same-name debuglink sections.  */
  /* FIXME: We do not check for the presence of a dwo link as well as a debuglink.  */

  check_for_and_load_links (file, filename);
  if (first_separate_info != NULL)
    return true;

  do_follow_links = 0;
  return false;
}