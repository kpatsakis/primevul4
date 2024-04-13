onig_region_resize(OnigRegion* region, int n)
{
  region->num_regs = n;

  if (n < ONIG_NREGION)
    n = ONIG_NREGION;

  if (region->allocated == 0) {
    region->beg = (int* )xmalloc(n * sizeof(int));
    region->end = (int* )xmalloc(n * sizeof(int));

    if (region->beg == 0 || region->end == 0)
      return ONIGERR_MEMORY;

    region->allocated = n;
  }
  else if (region->allocated < n) {
    region->beg = (int* )xrealloc(region->beg, n * sizeof(int));
    region->end = (int* )xrealloc(region->end, n * sizeof(int));

    if (region->beg == 0 || region->end == 0)
      return ONIGERR_MEMORY;

    region->allocated = n;
  }

  return 0;
}
