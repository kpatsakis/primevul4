dwarf_vmatoa (const char * fmtch, dwarf_vma value)
{
  return dwarf_vmatoa_1 (fmtch, value, 0);
}