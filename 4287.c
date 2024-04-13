mrb_ci_nregs(mrb_callinfo *ci)
{
  const struct RProc *p;

  if (!ci) return 4;
  mrb_int nregs = mrb_ci_bidx(ci) + 1; /* self + args + kargs + blk */
  p = ci->proc;
  if (p && !MRB_PROC_CFUNC_P(p) && p->body.irep && p->body.irep->nregs > nregs) {
    return p->body.irep->nregs;
  }
  return nregs;
}