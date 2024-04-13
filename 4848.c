init_dwarf_regnames_riscv (void)
{
  dwarf_regnames = NULL;
  dwarf_regnames_count = 8192;
  dwarf_regnames_lookup_func = regname_internal_riscv;
}