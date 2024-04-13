display_debug_lines (struct dwarf_section *section, void *file)
{
  unsigned char *data = section->start;
  unsigned char *end = data + section->size;
  int retValRaw = 1;
  int retValDecoded = 1;

  if (do_debug_lines == 0)
    do_debug_lines |= FLAG_DEBUG_LINES_RAW;

  if (do_debug_lines & FLAG_DEBUG_LINES_RAW)
    retValRaw = display_debug_lines_raw (section, data, end, file);

  if (do_debug_lines & FLAG_DEBUG_LINES_DECODED)
    retValDecoded = display_debug_lines_decoded (section, data, data, end, file);

  if (!retValRaw || !retValDecoded)
    return 0;

  return 1;
}