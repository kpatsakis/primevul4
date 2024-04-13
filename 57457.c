int yr_re_finalize_thread(void)
{
  RE_FIBER* fiber;
  RE_FIBER* next_fiber;
  RE_THREAD_STORAGE* storage;

  if (thread_storage_key != 0)
    storage = (RE_THREAD_STORAGE*) yr_thread_storage_get_value(
        &thread_storage_key);
  else
    return ERROR_SUCCESS;

  if (storage != NULL)
  {
    fiber = storage->fiber_pool.fibers.head;

    while (fiber != NULL)
    {
      next_fiber = fiber->next;
      yr_free(fiber);
      fiber = next_fiber;
    }

    yr_free(storage);
  }

  return yr_thread_storage_set_value(&thread_storage_key, NULL);
}
