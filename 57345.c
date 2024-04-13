mrb_realloc_simple(mrb_state *mrb, void *p,  size_t len)
{
  void *p2;

  p2 = (mrb->allocf)(mrb, p, len, mrb->allocf_ud);
  if (!p2 && len > 0 && mrb->gc.heaps) {
    mrb_full_gc(mrb);
    p2 = (mrb->allocf)(mrb, p, len, mrb->allocf_ud);
  }

  return p2;
}
