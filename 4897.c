find_cu_tu_set_v2 (dwarf_vma cu_offset, int do_types)
{
  struct cu_tu_set *p;
  unsigned int nsets;
  unsigned int dw_sect;

  if (do_types)
    {
      p = tu_sets;
      nsets = tu_count;
      dw_sect = DW_SECT_TYPES;
    }
  else
    {
      p = cu_sets;
      nsets = cu_count;
      dw_sect = DW_SECT_INFO;
    }
  while (nsets > 0)
    {
      if (p->section_offsets [dw_sect] == cu_offset)
	return p;
      p++;
      nsets--;
    }
  return NULL;
}