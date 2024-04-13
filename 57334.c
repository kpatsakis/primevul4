mrb_gc_protect(mrb_state *mrb, mrb_value obj)
{
  if (mrb_immediate_p(obj)) return;
  gc_protect(mrb, &mrb->gc, mrb_basic_ptr(obj));
}
