test_incremental_sweep_phase(void)
{
  mrb_state *mrb = mrb_open();
  mrb_gc *gc = &mrb->gc;

  puts("test_incremental_sweep_phase");

  add_heap(mrb, gc);
  gc->sweeps = gc->heaps;

  mrb_assert(gc->heaps->next->next == NULL);
  mrb_assert(gc->free_heaps->next->next == NULL);
  incremental_sweep_phase(mrb, gc, MRB_HEAP_PAGE_SIZE * 3);

  mrb_assert(gc->heaps->next == NULL);
  mrb_assert(gc->heaps == gc->free_heaps);

  mrb_close(mrb);
}
