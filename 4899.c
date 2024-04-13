print_dwarf_view (dwarf_vma value, unsigned num_bytes, int force)
{
  int len;
  if (!num_bytes)
    len = 4;
  else
    len = num_bytes * 2;

  assert (value == (unsigned long) value);
  if (value || force)
    printf ("v%0*lx ", len - 1, (unsigned long) value);
  else
    printf ("%*s", len + 1, "");
}