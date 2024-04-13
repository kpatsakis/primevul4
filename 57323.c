link_heap_page(mrb_gc *gc, mrb_heap_page *page)
{
  page->next = gc->heaps;
  if (gc->heaps)
    gc->heaps->prev = page;
  gc->heaps = page;
}
