ppmd_free(void *p, void *address)
{
  (void)p;
  free(address);
}
