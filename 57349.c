root_scan_phase(mrb_state *mrb, mrb_gc *gc)
{
  size_t i, e;

  if (!is_minor_gc(gc)) {
    gc->gray_list = NULL;
    gc->atomic_gray_list = NULL;
  }

  mrb_gc_mark_gv(mrb);
  /* mark arena */
  for (i=0,e=gc->arena_idx; i<e; i++) {
    mrb_gc_mark(mrb, gc->arena[i]);
  }
  /* mark class hierarchy */
  mrb_gc_mark(mrb, (struct RBasic*)mrb->object_class);

  /* mark built-in classes */
  mrb_gc_mark(mrb, (struct RBasic*)mrb->class_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->module_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->proc_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->string_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->array_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->hash_class);

  mrb_gc_mark(mrb, (struct RBasic*)mrb->float_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->fixnum_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->true_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->false_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->nil_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->symbol_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->kernel_module);

  mrb_gc_mark(mrb, (struct RBasic*)mrb->eException_class);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->eStandardError_class);

  /* mark top_self */
  mrb_gc_mark(mrb, (struct RBasic*)mrb->top_self);
  /* mark exception */
  mrb_gc_mark(mrb, (struct RBasic*)mrb->exc);
  /* mark backtrace */
  mrb_gc_mark(mrb, (struct RBasic*)mrb->backtrace.exc);
  e = (size_t)mrb->backtrace.n;
  for (i=0; i<e; i++) {
    mrb_gc_mark(mrb, (struct RBasic*)mrb->backtrace.entries[i].klass);
  }
  /* mark pre-allocated exception */
  mrb_gc_mark(mrb, (struct RBasic*)mrb->nomem_err);
  mrb_gc_mark(mrb, (struct RBasic*)mrb->stack_err);
#ifdef MRB_GC_FIXED_ARENA
  mrb_gc_mark(mrb, (struct RBasic*)mrb->arena_err);
#endif

  mark_context(mrb, mrb->root_c);
  if (mrb->root_c->fib) {
    mrb_gc_mark(mrb, (struct RBasic*)mrb->root_c->fib);
  }
  if (mrb->root_c != mrb->c) {
    mark_context(mrb, mrb->c);
  }
}
