mrb_full_gc(mrb_state *mrb)
{
  mrb_gc *gc = &mrb->gc;

  if (gc->disabled) return;

  GC_INVOKE_TIME_REPORT("mrb_full_gc()");
  GC_TIME_START;

  if (is_generational(gc)) {
    /* clear all the old objects back to young */
    clear_all_old(mrb, gc);
    gc->full = TRUE;
  }
  else if (gc->state != MRB_GC_STATE_ROOT) {
    /* finish half baked GC cycle */
    incremental_gc_until(mrb, gc, MRB_GC_STATE_ROOT);
  }

  incremental_gc_until(mrb, gc, MRB_GC_STATE_ROOT);
  gc->threshold = (gc->live_after_mark/100) * gc->interval_ratio;

  if (is_generational(gc)) {
    gc->majorgc_old_threshold = gc->live_after_mark/100 * DEFAULT_MAJOR_GC_INC_RATIO;
    gc->full = FALSE;
  }

  GC_TIME_STOP_AND_REPORT;
}
