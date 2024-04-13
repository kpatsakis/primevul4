init_dwarf_regnames_iamcu (void)
{
  dwarf_regnames = dwarf_regnames_iamcu;
  dwarf_regnames_count = ARRAY_SIZE (dwarf_regnames_iamcu);
  dwarf_regnames_lookup_func = regname_internal_by_table_only;
}