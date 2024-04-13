dwarf_select_sections_by_letters (const char *letters)
{
  int result = 0;

  while (* letters)
    {
      const debug_dump_long_opts *entry;

      for (entry = debug_option_table; entry->letter; entry++)
	{
	  if (entry->letter == * letters)
	    {
	      * entry->variable |= entry->val;
	      result |= entry->val;
	      break;
	    }
	}

      if (entry->letter == 0)
	warn (_("Unrecognized debug letter option '%c'\n"), * letters);

      letters ++;
    }

  /* The --debug-dump=frames-interp option also enables the
     --debug-dump=frames option.  */
  if (do_debug_frames_interp)
    do_debug_frames = 1;

  return result;
}