exec_irep(mrb_state *mrb, mrb_value self, struct RProc *p)
{
  mrb_callinfo *ci = mrb->c->ci;
  mrb_int keep, nregs;

  ci->stack[0] = self;
  mrb_vm_ci_proc_set(ci, p);
  if (MRB_PROC_CFUNC_P(p)) {
    if (MRB_PROC_NOARG_P(p)) {
      check_method_noarg(mrb, ci);
    }
    return MRB_PROC_CFUNC(p)(mrb, self);
  }
  nregs = p->body.irep->nregs;
  keep = mrb_ci_bidx(ci)+1;
  if (nregs < keep) {
    mrb_stack_extend(mrb, keep);
  }
  else {
    mrb_stack_extend(mrb, nregs);
    stack_clear(ci->stack+keep, nregs-keep);
  }

  cipush(mrb, 0, 0, NULL, NULL, 0, 0);

  return self;
}