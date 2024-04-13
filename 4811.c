display_cu_index (struct dwarf_section *section, void *file ATTRIBUTE_UNUSED)
{
  return process_cu_tu_index (section, 1);
}