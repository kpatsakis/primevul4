mrb_gc_init(mrb_state *mrb, mrb_gc *gc)
{
#ifndef MRB_GC_FIXED_ARENA
  gc->arena = (struct RBasic**)mrb_malloc(mrb, sizeof(struct RBasic*)*MRB_GC_ARENA_SIZE);
  gc->arena_capa = MRB_GC_ARENA_SIZE;
#endif

  gc->current_white_part = GC_WHITE_A;
  gc->heaps = NULL;
  gc->free_heaps = NULL;
  add_heap(mrb, gc);
  gc->interval_ratio = DEFAULT_GC_INTERVAL_RATIO;
  gc->step_ratio = DEFAULT_GC_STEP_RATIO;
#ifndef MRB_GC_TURN_OFF_GENERATIONAL
  gc->generational = TRUE;
  gc->full = TRUE;
#endif

#ifdef GC_PROFILE
  program_invoke_time = gettimeofday_time();
#endif
}
