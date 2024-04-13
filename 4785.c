get_DW_SECT_short_name (unsigned int dw_sect)
{
  static char buf[16];

  switch (dw_sect)
    {
      case DW_SECT_INFO:
	return "info";
      case DW_SECT_TYPES:
	return "types";
      case DW_SECT_ABBREV:
	return "abbrev";
      case DW_SECT_LINE:
	return "line";
      case DW_SECT_LOC:
	return "loc";
      case DW_SECT_STR_OFFSETS:
	return "str_off";
      case DW_SECT_MACINFO:
	return "macinfo";
      case DW_SECT_MACRO:
	return "macro";
      default:
	break;
    }

  snprintf (buf, sizeof (buf), "%d", dw_sect);
  return buf;
}