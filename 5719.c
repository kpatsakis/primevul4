mrb_class_inherited(mrb_state *mrb, struct RClass *super, struct RClass *klass)
{
  mrb_value s;
  mrb_sym mid;

  if (!super)
    super = mrb->object_class;
  super->flags |= MRB_FL_CLASS_IS_INHERITED;
  s = mrb_obj_value(super);
  mrb_mc_clear_by_class(mrb, klass);
  mid = MRB_SYM(inherited);
  if (!mrb_func_basic_p(mrb, s, mid, mrb_do_nothing)) {
    mrb_value c = mrb_obj_value(klass);
    mrb_funcall_argv(mrb, s, mid, 1, &c);
  }
}