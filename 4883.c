check_gnu_debugaltlink (const char * filename, void * data ATTRIBUTE_UNUSED)
{
  void * sep_data = open_debug_file (filename);

  if (sep_data == NULL)
    return false;

  /* FIXME: We should now extract the build-id in the separate file
     and check it...  */

  return true;
}