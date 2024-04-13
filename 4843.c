init_dwarf_regnames_x86_64 (void)
{
  dwarf_regnames = dwarf_regnames_x86_64;
  dwarf_regnames_count = ARRAY_SIZE (dwarf_regnames_x86_64);
  dwarf_regnames_lookup_func = regname_internal_by_table_only;
}