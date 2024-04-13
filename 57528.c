onig_region_copy(OnigRegion* to, OnigRegion* from)
{
#define RREGC_SIZE   (sizeof(int) * from->num_regs)
  int i;

  if (to == from) return;

  if (to->allocated == 0) {
    if (from->num_regs > 0) {
      to->beg = (int* )xmalloc(RREGC_SIZE);
      to->end = (int* )xmalloc(RREGC_SIZE);
      to->allocated = from->num_regs;
    }
  }
  else if (to->allocated < from->num_regs) {
    to->beg = (int* )xrealloc(to->beg, RREGC_SIZE);
    to->end = (int* )xrealloc(to->end, RREGC_SIZE);
    to->allocated = from->num_regs;
  }

  for (i = 0; i < from->num_regs; i++) {
    to->beg[i] = from->beg[i];
    to->end[i] = from->end[i];
  }
  to->num_regs = from->num_regs;

#ifdef USE_CAPTURE_HISTORY
  history_root_free(to);

  if (IS_NOT_NULL(from->history_root)) {
    to->history_root = history_tree_clone(from->history_root);
  }
#endif
}
