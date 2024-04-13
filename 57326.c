mrb_field_write_barrier(mrb_state *mrb, struct RBasic *obj, struct RBasic *value)
{
  mrb_gc *gc = &mrb->gc;

  if (!is_black(obj)) return;
  if (!is_white(value)) return;

  mrb_assert(gc->state == MRB_GC_STATE_MARK || (!is_dead(gc, value) && !is_dead(gc, obj)));
  mrb_assert(is_generational(gc) || gc->state != MRB_GC_STATE_ROOT);

  if (is_generational(gc) || gc->state == MRB_GC_STATE_MARK) {
    add_gray_list(mrb, gc, value);
  }
  else {
    mrb_assert(gc->state == MRB_GC_STATE_SWEEP);
    paint_partial_white(gc, obj); /* for never write barriers */
  }
}
