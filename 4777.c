range_entry_compar (const void *ap, const void *bp)
{
  const struct range_entry *a_re = (const struct range_entry *) ap;
  const struct range_entry *b_re = (const struct range_entry *) bp;
  const dwarf_vma a = a_re->ranges_offset;
  const dwarf_vma b = b_re->ranges_offset;

  return (a > b) - (b > a);
}