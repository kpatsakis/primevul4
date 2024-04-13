mrb_yield(mrb_state *mrb, mrb_value b, mrb_value arg)
{
  struct RProc *p = mrb_proc_ptr(b);

  return mrb_yield_with_class(mrb, b, 1, &arg, MRB_PROC_ENV(p)->stack[0], MRB_PROC_TARGET_CLASS(p));
}