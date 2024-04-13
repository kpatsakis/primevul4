int yr_re_finalize(void)
{
  yr_thread_storage_destroy(&thread_storage_key);

  thread_storage_key = 0;
  return ERROR_SUCCESS;
}
