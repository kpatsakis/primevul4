display_debug_gnu_pubnames (struct dwarf_section *section, void *file)
{
  return display_debug_pubnames_worker (section, file, 1);
}