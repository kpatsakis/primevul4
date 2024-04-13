mrb_yield_cont(mrb_state *mrb, mrb_value b, mrb_value self, mrb_int argc, const mrb_value *argv)
{
  struct RProc *p;
  mrb_callinfo *ci;

  check_block(mrb, b);
  p = mrb_proc_ptr(b);
  ci = mrb->c->ci;

  mrb_stack_extend(mrb, 4);
  mrb->c->ci->stack[1] = mrb_ary_new_from_values(mrb, argc, argv);
  mrb->c->ci->stack[2] = mrb_nil_value();
  mrb->c->ci->stack[3] = mrb_nil_value();
  ci->n = 15;
  ci->nk = 0;
  return exec_irep(mrb, self, p);
}