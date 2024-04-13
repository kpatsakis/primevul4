incremental_gc_until(mrb_state *mrb, mrb_gc *gc, mrb_gc_state to_state)
{
  do {
    incremental_gc(mrb, gc, SIZE_MAX);
  } while (gc->state != to_state);
}
