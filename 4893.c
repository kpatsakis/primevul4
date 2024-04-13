is_max_address (dwarf_vma addr, unsigned int pointer_size)
{
  dwarf_vma mask = ~(~(dwarf_vma) 1 << (pointer_size * 8 - 1));
  return ((addr & mask) == mask);
}