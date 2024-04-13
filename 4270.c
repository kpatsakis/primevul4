mrb_obj_instance_eval(mrb_state *mrb, mrb_value self)
{
  mrb_value a, b;

  if (mrb_get_args(mrb, "|S&", &a, &b) == 1) {
    mrb_raise(mrb, E_NOTIMP_ERROR, "instance_eval with string not implemented");
  }
  return eval_under(mrb, self, b, mrb_singleton_class_ptr(mrb, self));
}