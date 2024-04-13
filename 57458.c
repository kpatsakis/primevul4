int yr_re_initialize(void)
{
  return yr_thread_storage_create(&thread_storage_key);
}
