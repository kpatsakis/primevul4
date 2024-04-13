zfree_cb(voidpf opaque, voidpf ptr)
{
  (void) opaque;
  free(ptr);
}