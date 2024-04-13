find_debug_info_for_offset (dwarf_vma offset)
{
  unsigned int i;

  if (num_debug_info_entries == DEBUG_INFO_UNAVAILABLE)
    return NULL;

  for (i = 0; i < num_debug_info_entries; i++)
    if (debug_information[i].cu_offset == offset)
      return debug_information + i;

  return NULL;
}