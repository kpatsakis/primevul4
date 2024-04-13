mrb_exec_irep(mrb_state *mrb, mrb_value self, struct RProc *p)
{
  mrb_callinfo *ci = mrb->c->ci;
  if (ci->cci == CINFO_NONE) {
    return exec_irep(mrb, self, p);
  }
  else {
    mrb_value ret;
    if (MRB_PROC_CFUNC_P(p)) {
      if (MRB_PROC_NOARG_P(p)) {
        check_method_noarg(mrb, ci);
      }
      cipush(mrb, 0, CINFO_DIRECT, mrb_vm_ci_target_class(ci), p, ci->mid, ci->n|(ci->nk<<4));
      ret = MRB_PROC_CFUNC(p)(mrb, self);
      cipop(mrb);
    }
    else {
      mrb_int keep = mrb_ci_bidx(ci) + 1; /* receiver + block */
      ret = mrb_top_run(mrb, p, self, keep);
    }
    if (mrb->exc && mrb->jmp) {
      mrb_exc_raise(mrb, mrb_obj_value(mrb->exc));
    }
    return ret;
  }
}