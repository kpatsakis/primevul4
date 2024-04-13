dwarf_select_sections_all (void)
{
  do_debug_info = 1;
  do_debug_abbrevs = 1;
  do_debug_lines = FLAG_DEBUG_LINES_RAW;
  do_debug_pubnames = 1;
  do_debug_pubtypes = 1;
  do_debug_aranges = 1;
  do_debug_ranges = 1;
  do_debug_frames = 1;
  do_debug_macinfo = 1;
  do_debug_str = 1;
  do_debug_loc = 1;
  do_gdb_index = 1;
  do_trace_info = 1;
  do_trace_abbrevs = 1;
  do_trace_aranges = 1;
  do_debug_addr = 1;
  do_debug_cu_index = 1;
  do_follow_links = 1;
  do_debug_links = 1;
  do_debug_str_offsets = 1;
}