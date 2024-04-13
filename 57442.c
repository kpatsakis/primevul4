int _yr_re_fiber_exists(
    RE_FIBER_LIST* fiber_list,
    RE_FIBER* target_fiber,
    RE_FIBER* last_fiber)
{
  RE_FIBER* fiber = fiber_list->head;

  int equal_stacks;
  int i;


  if (last_fiber == NULL)
    return FALSE;

  while (fiber != last_fiber->next)
  {
    if (fiber->ip == target_fiber->ip &&
        fiber->sp == target_fiber->sp &&
        fiber->rc == target_fiber->rc)
    {
      equal_stacks = TRUE;

      for (i = 0; i <= fiber->sp; i++)
      {
        if (fiber->stack[i] != target_fiber->stack[i])
        {
          equal_stacks = FALSE;
          break;
        }
      }

      if (equal_stacks)
        return TRUE;
    }

    fiber = fiber->next;
  }

  return FALSE;
}
