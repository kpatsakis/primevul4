mrb_gc_mark(mrb_state *mrb, struct RBasic *obj)
{
  if (obj == 0) return;
  if (!is_white(obj)) return;
  mrb_assert((obj)->tt != MRB_TT_FREE);
  add_gray_list(mrb, &mrb->gc, obj);
}
