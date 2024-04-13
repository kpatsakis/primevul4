record_abbrev_list_for_cu (dwarf_vma start, dwarf_vma end, abbrev_list * list)
{
  if (cu_abbrev_map == NULL)
    {
      num_abbrev_map_entries = INITIAL_NUM_ABBREV_MAP_ENTRIES;
      cu_abbrev_map = xmalloc (num_abbrev_map_entries * sizeof (* cu_abbrev_map));
    }
  else if (next_free_abbrev_map_entry == num_abbrev_map_entries)
    {
      num_abbrev_map_entries += ABBREV_MAP_ENTRIES_INCREMENT;
      cu_abbrev_map = xrealloc (cu_abbrev_map, num_abbrev_map_entries * sizeof (* cu_abbrev_map));
    }

  cu_abbrev_map[next_free_abbrev_map_entry].start = start;
  cu_abbrev_map[next_free_abbrev_map_entry].end = end;
  cu_abbrev_map[next_free_abbrev_map_entry].list = list;
  next_free_abbrev_map_entry ++;
}