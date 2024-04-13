mrb_ci_bidx(mrb_callinfo *ci)
{
  return mrb_bidx(ci->n|(ci->nk<<4));
}