mrb_calloc(mrb_state *mrb, size_t nelem, size_t len)
{
  void *p;

  if (nelem > 0 && len > 0 &&
      nelem <= SIZE_MAX / len) {
    size_t size;
    size = nelem * len;
    p = mrb_malloc(mrb, size);

    memset(p, 0, size);
  }
  else {
    p = NULL;
  }

  return p;
}
