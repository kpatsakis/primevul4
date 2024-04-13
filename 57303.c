change_gen_gc_mode(mrb_state *mrb, mrb_gc *gc, mrb_bool enable)
{
  if (is_generational(gc) && !enable) {
    clear_all_old(mrb, gc);
    mrb_assert(gc->state == MRB_GC_STATE_ROOT);
    gc->full = FALSE;
  }
  else if (!is_generational(gc) && enable) {
    incremental_gc_until(mrb, gc, MRB_GC_STATE_ROOT);
    gc->majorgc_old_threshold = gc->live_after_mark/100 * DEFAULT_MAJOR_GC_INC_RATIO;
    gc->full = FALSE;
  }
  gc->generational = enable;
}
