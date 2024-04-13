incremental_gc_step(mrb_state *mrb, mrb_gc *gc)
{
  size_t limit = 0, result = 0;
  limit = (GC_STEP_SIZE/100) * gc->step_ratio;
  while (result < limit) {
    result += incremental_gc(mrb, gc, limit);
    if (gc->state == MRB_GC_STATE_ROOT)
      break;
  }

  gc->threshold = gc->live + GC_STEP_SIZE;
}
