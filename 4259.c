eval_under(mrb_state *mrb, mrb_value self, mrb_value blk, struct RClass *c)
{
  struct RProc *p;
  mrb_callinfo *ci;
  int nregs;

  check_block(mrb, blk);
  ci = mrb->c->ci;
  if (ci->cci == CINFO_DIRECT) {
    return mrb_yield_with_class(mrb, blk, 1, &self, self, c);
  }
  ci->u.target_class = c;
  p = mrb_proc_ptr(blk);
  mrb_vm_ci_proc_set(ci, p);
  ci->n = 1;
  ci->nk = 0;
  ci->mid = ci[-1].mid;
  if (MRB_PROC_CFUNC_P(p)) {
    mrb_stack_extend(mrb, 4);
    mrb->c->ci->stack[0] = self;
    mrb->c->ci->stack[1] = self;
    mrb->c->ci->stack[2] = mrb_nil_value();
    return MRB_PROC_CFUNC(p)(mrb, self);
  }
  nregs = p->body.irep->nregs;
  if (nregs < 4) nregs = 4;
  mrb_stack_extend(mrb, nregs);
  mrb->c->ci->stack[0] = self;
  mrb->c->ci->stack[1] = self;
  stack_clear(mrb->c->ci->stack+2, nregs-2);
  ci = cipush(mrb, 0, 0, NULL, NULL, 0, 0);

  return self;
}