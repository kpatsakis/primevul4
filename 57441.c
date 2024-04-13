int _yr_re_fiber_create(
    RE_FIBER_POOL* fiber_pool,
    RE_FIBER** new_fiber)
{
  RE_FIBER* fiber;

  if (fiber_pool->fibers.head != NULL)
  {
    fiber = fiber_pool->fibers.head;
    fiber_pool->fibers.head = fiber->next;

    if (fiber_pool->fibers.tail == fiber)
      fiber_pool->fibers.tail = NULL;
  }
  else
  {
    if (fiber_pool->fiber_count == RE_MAX_FIBERS)
      return ERROR_TOO_MANY_RE_FIBERS;

    fiber = (RE_FIBER*) yr_malloc(sizeof(RE_FIBER));

    if (fiber == NULL)
      return ERROR_INSUFFICIENT_MEMORY;

    fiber_pool->fiber_count++;
  }

  fiber->ip = NULL;
  fiber->sp = -1;
  fiber->rc = -1;
  fiber->next = NULL;
  fiber->prev = NULL;

  *new_fiber = fiber;

  return ERROR_SUCCESS;
}
