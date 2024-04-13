gc_generational_mode_set(mrb_state *mrb, mrb_value self)
{
  mrb_bool enable;

  mrb_get_args(mrb, "b", &enable);
  if (mrb->gc.generational != enable)
    change_gen_gc_mode(mrb, &mrb->gc, enable);

  return mrb_bool_value(enable);
}
