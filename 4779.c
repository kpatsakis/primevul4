regname_internal_by_table_only (unsigned int regno)
{
  if (dwarf_regnames != NULL
      && regno < dwarf_regnames_count
      && dwarf_regnames [regno] != NULL)
    return dwarf_regnames [regno];

  return NULL;
}