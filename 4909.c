dwarf_select_sections_by_names (const char *names)
{
  const char *p;
  int result = 0;

  p = names;
  while (*p)
    {
      const debug_dump_long_opts *entry;

      for (entry = debug_option_table; entry->option; entry++)
	{
	  size_t len = strlen (entry->option);

	  if (strncmp (p, entry->option, len) == 0
	      && (p[len] == ',' || p[len] == '\0'))
	    {
	      * entry->variable = entry->val;
	      result |= entry->val;

	      p += len;
	      break;
	    }
	}

      if (entry->option == NULL)
	{
	  warn (_("Unrecognized debug option '%s'\n"), p);
	  p = strchr (p, ',');
	  if (p == NULL)
	    break;
	}

      if (*p == ',')
	p++;
    }

  /* The --debug-dump=frames-interp option also enables the
     --debug-dump=frames option.  */
  if (do_debug_frames_interp)
    do_debug_frames = 1;

  return result;
}