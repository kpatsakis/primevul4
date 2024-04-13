onig_region_free(OnigRegion* r, int free_self)
{
  if (r) {
    if (r->allocated > 0) {
      if (r->beg) xfree(r->beg);
      if (r->end) xfree(r->end);
      r->allocated = 0;
    }
#ifdef USE_CAPTURE_HISTORY
    history_root_free(r);
#endif
    if (free_self) xfree(r);
  }
}
