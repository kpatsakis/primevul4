mrb_yield_with_class(mrb_state *mrb, mrb_value b, mrb_int argc, const mrb_value *argv, mrb_value self, struct RClass *c)
{
  struct RProc *p;
  mrb_sym mid = mrb->c->ci->mid;
  mrb_callinfo *ci;
  mrb_value val;
  mrb_int n;

  check_block(mrb, b);
  ci = mrb->c->ci;
  n = mrb_ci_nregs(ci);
  p = mrb_proc_ptr(b);
  ci = cipush(mrb, n, CINFO_SKIP, c, p, mid, 0 /* dummy */);
  ci->nk = 0;
  if (argc >= CALL_MAXARGS) {
    ci->n = 15;
    n = 3;
  }
  else {
    ci->n = (uint8_t)argc;
    n = argc + 2;
  }
  mrb_stack_extend(mrb, n);
  mrb->c->ci->stack[0] = self;
  if (ci->n == 15) {
    mrb->c->ci->stack[1] = mrb_ary_new_from_values(mrb, argc, argv);
    argc = 1;
  }
  else if (argc > 0) {
    stack_copy(mrb->c->ci->stack+1, argv, argc);
  }
  mrb->c->ci->stack[argc+1] = mrb_nil_value(); /* clear blk */

  if (MRB_PROC_CFUNC_P(p)) {
    ci->cci = CINFO_DIRECT;
    val = MRB_PROC_CFUNC(p)(mrb, self);
    cipop(mrb);
  }
  else {
    val = mrb_run(mrb, p, self);
  }
  return val;
}