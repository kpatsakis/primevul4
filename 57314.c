gc_start(mrb_state *mrb, mrb_value obj)
{
  mrb_full_gc(mrb);
  return mrb_nil_value();
}
