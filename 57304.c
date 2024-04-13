clear_all_old(mrb_state *mrb, mrb_gc *gc)
{
  mrb_bool origin_mode = gc->generational;

  mrb_assert(is_generational(gc));
  if (is_major_gc(gc)) {
    /* finish the half baked GC */
    incremental_gc_until(mrb, gc, MRB_GC_STATE_ROOT);
  }

  /* Sweep the dead objects, then reset all the live objects
   * (including all the old objects, of course) to white. */
  gc->generational = FALSE;
  prepare_incremental_sweep(mrb, gc);
  incremental_gc_until(mrb, gc, MRB_GC_STATE_ROOT);
  gc->generational = origin_mode;

  /* The gray objects have already been painted as white */
  gc->atomic_gray_list = gc->gray_list = NULL;
}
