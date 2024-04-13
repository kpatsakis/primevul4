load_debug_section_with_follow (enum dwarf_section_display_enum sec_enum,
				void * handle)
{
  if (load_debug_section (sec_enum, handle))
    {
      if (debug_displays[sec_enum].section.filename == NULL)
	{
	  /* See if we can associate a filename with this section.  */
	  separate_info * i;

	  for (i = first_separate_info; i != NULL; i = i->next)
	    if (i->handle == handle)
	      {
		debug_displays[sec_enum].section.filename = i->filename;
		break;
	      }
	}

      return true;
    }

  if (do_follow_links)
    {
      separate_info * i;

      for (i = first_separate_info; i != NULL; i = i->next)
	{
	  if (load_debug_section (sec_enum, i->handle))
	    {
	      debug_displays[sec_enum].section.filename = i->filename;

	      /* FIXME: We should check to see if any of the remaining debug info
		 files also contain this section, and, umm, do something about it.  */
	      return true;
	    }
	}
    }

  return false;
}