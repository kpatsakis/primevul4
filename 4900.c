load_dwo_file (const char * main_filename, const char * name, const char * dir, const char * id ATTRIBUTE_UNUSED)
{
  char * separate_filename;
  void * separate_handle;

  if (IS_ABSOLUTE_PATH (name))
    separate_filename = strdup (name);
  else
    /* FIXME: Skip adding / if dwo_dir ends in /.  */
    separate_filename = concat (dir, "/", name, NULL);
  if (separate_filename == NULL)
    {
      warn (_("Out of memory allocating dwo filename\n"));
      return NULL;
    }

  if ((separate_handle = open_debug_file (separate_filename)) == NULL)
    {
      warn (_("Unable to load dwo file: %s\n"), separate_filename);
      free (separate_filename);
      return NULL;
    }

  /* FIXME: We should check the dwo_id.  */

  printf (_("%s: Found separate debug object file: %s\n\n"), main_filename, separate_filename);

  add_separate_debug_file (separate_filename, separate_handle);
  /* Note - separate_filename will be freed in free_debug_memory().  */
  return separate_handle;
}