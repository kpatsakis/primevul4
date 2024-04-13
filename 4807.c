load_debug_sup_file (const char * main_filename, void * file)
{
  if (! load_debug_section (debug_sup, file))
    return; /* No .debug_sup section.  */

  struct dwarf_section * section;
  section = & debug_displays [debug_sup].section;
  assert (section != NULL);

  if (section->start == NULL || section->size < 5)
    {
      warn (_(".debug_sup section is corrupt/empty\n"));
      return;
    }

  if (section->start[2] != 0)
    return; /* This is a supplementary file.  */

  const char * filename = (const char *) section->start + 3;
  if (strnlen (filename, section->size - 3) == section->size - 3)
    {
      warn (_("filename in .debug_sup section is corrupt\n"));
      return;
    }

  if (filename[0] != '/' && strchr (main_filename, '/'))
    {
      char * new_name;
      int new_len;

      new_len = asprintf (& new_name, "%.*s/%s",
			  (int) (strrchr (main_filename, '/') - main_filename),
			  main_filename,
			  filename);
      if (new_len < 3)
	{
	  warn (_("unable to construct path for supplementary debug file"));
	  if (new_len > -1)
	    free (new_name);
	  return;
	}
      filename = new_name;
    }
  else
    {
      /* PR 27796: Make sure that we pass a filename that can be free'd to
	 add_separate_debug_file().  */
      filename = strdup (filename);
      if (filename == NULL)
	{
	  warn (_("out of memory constructing filename for .debug_sup link\n"));
	  return;
	}
    }

  void * handle = open_debug_file (filename);
  if (handle == NULL)
    {
      warn (_("unable to open file '%s' referenced from .debug_sup section\n"), filename);
      free ((void *) filename);
      return;
    }

  printf (_("%s: Found supplementary debug file: %s\n\n"), main_filename, filename);

  /* FIXME: Compare the checksums, if present.  */
  add_separate_debug_file (filename, handle);
}