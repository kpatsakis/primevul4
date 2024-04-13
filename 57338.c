mrb_init_gc(mrb_state *mrb)
{
  struct RClass *gc;

  gc = mrb_define_module(mrb, "GC");

  mrb_define_class_method(mrb, gc, "start", gc_start, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, gc, "enable", gc_enable, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, gc, "disable", gc_disable, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, gc, "interval_ratio", gc_interval_ratio_get, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, gc, "interval_ratio=", gc_interval_ratio_set, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, gc, "step_ratio", gc_step_ratio_get, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, gc, "step_ratio=", gc_step_ratio_set, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, gc, "generational_mode=", gc_generational_mode_set, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, gc, "generational_mode", gc_generational_mode_get, MRB_ARGS_NONE());
#ifdef GC_TEST
#ifdef GC_DEBUG
  mrb_define_class_method(mrb, gc, "test", gc_test, MRB_ARGS_NONE());
#endif
#endif
}
