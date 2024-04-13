int _yr_re_fiber_split(
    RE_FIBER_LIST* fiber_list,
    RE_FIBER_POOL* fiber_pool,
    RE_FIBER* fiber,
    RE_FIBER** new_fiber)
{
  int32_t i;

  FAIL_ON_ERROR(_yr_re_fiber_create(fiber_pool, new_fiber));

  (*new_fiber)->sp = fiber->sp;
  (*new_fiber)->ip = fiber->ip;
  (*new_fiber)->rc = fiber->rc;

  for (i = 0; i <= fiber->sp; i++)
    (*new_fiber)->stack[i] = fiber->stack[i];

  (*new_fiber)->next = fiber->next;
  (*new_fiber)->prev = fiber;

  if (fiber->next != NULL)
    fiber->next->prev = *new_fiber;

  fiber->next = *new_fiber;

  if (fiber_list->tail == fiber)
    fiber_list->tail = *new_fiber;

  assert(fiber_list->tail->next == NULL);
  assert(fiber_list->head->prev == NULL);

  return ERROR_SUCCESS;
}
