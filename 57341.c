mrb_obj_alloc(mrb_state *mrb, enum mrb_vtype ttype, struct RClass *cls)
{
  struct RBasic *p;
  static const RVALUE RVALUE_zero = { { { MRB_TT_FALSE } } };
  mrb_gc *gc = &mrb->gc;

  if (cls) {
    enum mrb_vtype tt;

    switch (cls->tt) {
    case MRB_TT_CLASS:
    case MRB_TT_SCLASS:
    case MRB_TT_MODULE:
    case MRB_TT_ENV:
      break;
    default:
      mrb_raise(mrb, E_TYPE_ERROR, "allocation failure");
    }
    tt = MRB_INSTANCE_TT(cls);
    if (tt != MRB_TT_FALSE &&
        ttype != MRB_TT_SCLASS &&
        ttype != MRB_TT_ICLASS &&
        ttype != MRB_TT_ENV &&
        ttype != tt) {
      mrb_raisef(mrb, E_TYPE_ERROR, "allocation failure of %S", mrb_obj_value(cls));
    }
  }

#ifdef MRB_GC_STRESS
  mrb_full_gc(mrb);
#endif
  if (gc->threshold < gc->live) {
    mrb_incremental_gc(mrb);
  }
  if (gc->free_heaps == NULL) {
    add_heap(mrb, gc);
  }

  p = gc->free_heaps->freelist;
  gc->free_heaps->freelist = ((struct free_obj*)p)->next;
  if (gc->free_heaps->freelist == NULL) {
    unlink_free_heap_page(gc, gc->free_heaps);
  }

  gc->live++;
  gc_protect(mrb, gc, p);
  *(RVALUE *)p = RVALUE_zero;
  p->tt = ttype;
  p->c = cls;
  paint_partial_white(gc, p);
  return p;
}
