mrb_obj_new(mrb_state *mrb, struct RClass *c, mrb_int argc, const mrb_value *argv)
{
  mrb_value obj;
  mrb_sym mid;

  obj = mrb_instance_alloc(mrb, mrb_obj_value(c));
  mid = MRB_SYM(initialize);
  if (!mrb_func_basic_p(mrb, obj, mid, mrb_do_nothing)) {
    mrb_funcall_argv(mrb, obj, mid, argc, argv);
  }
  return obj;
}