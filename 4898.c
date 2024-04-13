print_dwarf_vma (dwarf_vma value, unsigned num_bytes)
{
  printf ("%s ", dwarf_vmatoa_1 (NULL, value, num_bytes));
}