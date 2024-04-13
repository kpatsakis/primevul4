free_dwo_info (void)
{
  dwo_info * dwinfo;
  dwo_info * next;

  for (dwinfo = first_dwo_info; dwinfo != NULL; dwinfo = next)
    {
      next = dwinfo->next;
      free (dwinfo);
    }
  first_dwo_info = NULL;
}