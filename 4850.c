find_abbrev_map_by_offset (dwarf_vma offset)
{
  unsigned long i;

  for (i = 0; i < next_free_abbrev_map_entry; i++)
    if (cu_abbrev_map[i].start <= offset
	&& cu_abbrev_map[i].end > offset)
      return cu_abbrev_map + i;

  return NULL;	
}