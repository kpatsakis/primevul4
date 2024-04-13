compare_guards_by_confirmed_idx(const void **a_, const void **b_)
{
  const entry_guard_t *a = *a_, *b = *b_;
  if (a->confirmed_idx < b->confirmed_idx)
    return -1;
  else if (a->confirmed_idx > b->confirmed_idx)
    return 1;
  else
    return 0;
}
