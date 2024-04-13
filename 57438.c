int _yr_re_alloc_storage(
    RE_THREAD_STORAGE** storage)
{
  *storage = (RE_THREAD_STORAGE*) yr_thread_storage_get_value(
      &thread_storage_key);

  if (*storage == NULL)
  {
    *storage = (RE_THREAD_STORAGE*) yr_malloc(sizeof(RE_THREAD_STORAGE));

    if (*storage == NULL)
      return ERROR_INSUFFICIENT_MEMORY;

    (*storage)->fiber_pool.fiber_count = 0;
    (*storage)->fiber_pool.fibers.head = NULL;
    (*storage)->fiber_pool.fibers.tail = NULL;

    FAIL_ON_ERROR(
        yr_thread_storage_set_value(&thread_storage_key, *storage));
  }

  return ERROR_SUCCESS;
}
