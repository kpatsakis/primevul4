void _yr_re_fiber_kill_all(
    RE_FIBER_LIST* fiber_list,
    RE_FIBER_POOL* fiber_pool)
{
  if (fiber_list->head != NULL)
    _yr_re_fiber_kill_tail(fiber_list, fiber_pool, fiber_list->head);
}
