print_addr_index (unsigned int idx, unsigned int len)
{
  static char buf[15];
  snprintf (buf, sizeof (buf), "[%d]", idx);
  printf ("%*s ", len, buf);
}