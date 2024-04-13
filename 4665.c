zalloc_cb(voidpf opaque, unsigned int items, unsigned int size)
{
  (void) opaque;
  /* not a typo, keep it calloc() */
  return (voidpf) calloc(items, size);
}