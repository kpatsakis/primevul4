parse_gnu_debugaltlink (struct dwarf_section * section, void * data)
{
  const char *     name;
  bfd_size_type    namelen;
  bfd_size_type    id_len;
  Build_id_data *  build_id_data;

  /* The name is first.
     The build-id follows immediately, with no padding, up to the section's end.  */

  name = (const char *) section->start;
  namelen = strnlen (name, section->size) + 1;
  if (namelen == 1)
    return NULL;
  if (namelen >= section->size)
    return NULL;

  id_len = section->size - namelen;
  if (id_len < 0x14)
    return NULL;

  build_id_data = (Build_id_data *) data;
  build_id_data->len = id_len;
  build_id_data->data = section->start + namelen;

  return name;
}