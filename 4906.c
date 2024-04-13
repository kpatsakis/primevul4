read_and_print_leb128 (unsigned char *data,
		       unsigned int *bytes_read,
		       unsigned const char *end,
		       bool is_signed)
{
  int status;
  dwarf_vma val = read_leb128 (data, end, is_signed, bytes_read, &status);
  if (status != 0)
    report_leb_status (status);
  else
    printf ("%s", dwarf_vmatoa (is_signed ? "d" : "u", val));
}