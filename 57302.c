add_heap(mrb_state *mrb, mrb_gc *gc)
{
  mrb_heap_page *page = (mrb_heap_page *)mrb_calloc(mrb, 1, sizeof(mrb_heap_page) + MRB_HEAP_PAGE_SIZE * sizeof(RVALUE));
  RVALUE *p, *e;
  struct RBasic *prev = NULL;

  for (p = objects(page), e=p+MRB_HEAP_PAGE_SIZE; p<e; p++) {
    p->as.free.tt = MRB_TT_FREE;
    p->as.free.next = prev;
    prev = &p->as.basic;
  }
  page->freelist = prev;

  link_heap_page(gc, page);
  link_free_heap_page(gc, page);
}
