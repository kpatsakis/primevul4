void _yr_re_fiber_kill_tail(
  RE_FIBER_LIST* fiber_list,
  RE_FIBER_POOL* fiber_pool,
  RE_FIBER* fiber)
{
  RE_FIBER* prev_fiber = fiber->prev;

  if (prev_fiber != NULL)
    prev_fiber->next = NULL;

  fiber->prev = fiber_pool->fibers.tail;

  if (fiber_pool->fibers.tail != NULL)
    fiber_pool->fibers.tail->next = fiber;

  fiber_pool->fibers.tail = fiber_list->tail;
  fiber_list->tail = prev_fiber;

  if (fiber_list->head == fiber)
    fiber_list->head = NULL;

  if (fiber_pool->fibers.head == NULL)
    fiber_pool->fibers.head = fiber;
}
