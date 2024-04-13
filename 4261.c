mrb_yield_argv(mrb_state *mrb, mrb_value b, mrb_int argc, const mrb_value *argv)
{
  struct RProc *p = mrb_proc_ptr(b);

  return mrb_yield_with_class(mrb, b, argc, argv, MRB_PROC_ENV(p)->stack[0], MRB_PROC_TARGET_CLASS(p));
}