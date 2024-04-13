loc_offsets_compar (const void *ap, const void *bp)
{
  dwarf_vma a = loc_offsets[*(const unsigned int *) ap];
  dwarf_vma b = loc_offsets[*(const unsigned int *) bp];

  int ret = (a > b) - (b > a);
  if (ret)
    return ret;

  a = loc_views[*(const unsigned int *) ap];
  b = loc_views[*(const unsigned int *) bp];

  ret = (a > b) - (b > a);

  return ret;
}