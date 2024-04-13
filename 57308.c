gc_generational_mode_get(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(mrb->gc.generational);
}
