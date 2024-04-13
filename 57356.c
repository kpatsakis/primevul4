unlink_heap_page(mrb_gc *gc, mrb_heap_page *page)
{
  if (page->prev)
    page->prev->next = page->next;
  if (page->next)
    page->next->prev = page->prev;
  if (gc->heaps == page)
    gc->heaps = page->next;
  page->prev = NULL;
  page->next = NULL;
}
