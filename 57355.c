unlink_free_heap_page(mrb_gc *gc, mrb_heap_page *page)
{
  if (page->free_prev)
    page->free_prev->free_next = page->free_next;
  if (page->free_next)
    page->free_next->free_prev = page->free_prev;
  if (gc->free_heaps == page)
    gc->free_heaps = page->free_next;
  page->free_prev = NULL;
  page->free_next = NULL;
}
