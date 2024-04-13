incremental_gc(mrb_state *mrb, mrb_gc *gc, size_t limit)
{
  switch (gc->state) {
  case MRB_GC_STATE_ROOT:
    root_scan_phase(mrb, gc);
    gc->state = MRB_GC_STATE_MARK;
    flip_white_part(gc);
    return 0;
  case MRB_GC_STATE_MARK:
    if (gc->gray_list) {
      return incremental_marking_phase(mrb, gc, limit);
    }
    else {
      final_marking_phase(mrb, gc);
      prepare_incremental_sweep(mrb, gc);
      return 0;
    }
  case MRB_GC_STATE_SWEEP: {
     size_t tried_sweep = 0;
     tried_sweep = incremental_sweep_phase(mrb, gc, limit);
     if (tried_sweep == 0)
       gc->state = MRB_GC_STATE_ROOT;
     return tried_sweep;
  }
  default:
    /* unknown state */
    mrb_assert(0);
    return 0;
  }
}
