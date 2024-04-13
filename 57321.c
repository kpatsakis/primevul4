incremental_sweep_phase(mrb_state *mrb, mrb_gc *gc, size_t limit)
{
  mrb_heap_page *page = gc->sweeps;
  size_t tried_sweep = 0;

  while (page && (tried_sweep < limit)) {
    RVALUE *p = objects(page);
    RVALUE *e = p + MRB_HEAP_PAGE_SIZE;
    size_t freed = 0;
    mrb_bool dead_slot = TRUE;
    mrb_bool full = (page->freelist == NULL);

    if (is_minor_gc(gc) && page->old) {
      /* skip a slot which doesn't contain any young object */
      p = e;
      dead_slot = FALSE;
    }
    while (p<e) {
      if (is_dead(gc, &p->as.basic)) {
        if (p->as.basic.tt != MRB_TT_FREE) {
          obj_free(mrb, &p->as.basic, FALSE);
          if (p->as.basic.tt == MRB_TT_FREE) {
            p->as.free.next = page->freelist;
            page->freelist = (struct RBasic*)p;
            freed++;
          }
          else {
            dead_slot = FALSE;
          }
        }
      }
      else {
        if (!is_generational(gc))
          paint_partial_white(gc, &p->as.basic); /* next gc target */
        dead_slot = FALSE;
      }
      p++;
    }

    /* free dead slot */
    if (dead_slot && freed < MRB_HEAP_PAGE_SIZE) {
      mrb_heap_page *next = page->next;

      unlink_heap_page(gc, page);
      unlink_free_heap_page(gc, page);
      mrb_free(mrb, page);
      page = next;
    }
    else {
      if (full && freed > 0) {
        link_free_heap_page(gc, page);
      }
      if (page->freelist == NULL && is_minor_gc(gc))
        page->old = TRUE;
      else
        page->old = FALSE;
      page = page->next;
    }
    tried_sweep += MRB_HEAP_PAGE_SIZE;
    gc->live -= freed;
    gc->live_after_mark -= freed;
  }
  gc->sweeps = page;
  return tried_sweep;
}
