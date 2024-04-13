display_trace_info (struct dwarf_section *section, void *file)
{
  return process_debug_info (section, file, section->abbrev_sec, false, true);
}