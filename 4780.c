dwarf_vmatoa_1 (const char *fmtch, dwarf_vma value, unsigned num_bytes)
{
  /* As dwarf_vmatoa is used more then once in a printf call
     for output, we are cycling through a fixed array of pointers
     for return address.  */
  static int buf_pos = 0;
  static struct dwarf_vmatoa_buf
  {
    char place[64];
  } buf[16];
  char *ret;

  ret = buf[buf_pos++].place;
  buf_pos %= ARRAY_SIZE (buf);

  if (num_bytes)
    {
      /* Printf does not have a way of specifying a maximum field width for an
	 integer value, so we print the full value into a buffer and then select
	 the precision we need.  */
      snprintf (ret, sizeof (buf[0].place), DWARF_VMA_FMT_LONG, value);
      if (num_bytes > 8)
	num_bytes = 8;
      return ret + (16 - 2 * num_bytes);
    }
  else
    {
      char fmt[32];

      if (fmtch)
	sprintf (fmt, "%%%s%s", DWARF_VMA_FMT, fmtch);
      else
	sprintf (fmt, "%%%s", DWARF_VMA_FMT);
      snprintf (ret, sizeof (buf[0].place), fmt, value);
      return ret;
    }
}