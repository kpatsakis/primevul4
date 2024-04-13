add_separate_debug_file (const char * filename, void * handle)
{
  separate_info * i = xmalloc (sizeof * i);

  i->filename = filename;
  i->handle   = handle;
  i->next     = first_separate_info;
  first_separate_info = i;
}