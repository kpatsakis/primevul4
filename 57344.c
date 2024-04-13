mrb_realloc(mrb_state *mrb, void *p, size_t len)
{
  void *p2;

  p2 = mrb_realloc_simple(mrb, p, len);
  if (!p2 && len) {
    if (mrb->gc.out_of_memory) {
      mrb_exc_raise(mrb, mrb_obj_value(mrb->nomem_err));
      /* mrb_panic(mrb); */
    }
    else {
      mrb->gc.out_of_memory = TRUE;
      mrb_exc_raise(mrb, mrb_obj_value(mrb->nomem_err));
    }
  }
  else {
    mrb->gc.out_of_memory = FALSE;
  }

  return p2;
}
