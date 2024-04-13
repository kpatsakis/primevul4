add_dwo_info (const char * value, dwarf_vma cu_offset, dwo_type type)
{
  dwo_info * dwinfo = xmalloc (sizeof * dwinfo);

  dwinfo->type   = type;
  dwinfo->value  = value;
  dwinfo->cu_offset = cu_offset;
  dwinfo->next   = first_dwo_info;
  first_dwo_info = dwinfo;
}