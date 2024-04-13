mrb_malloc_simple(mrb_state *mrb, size_t len)
{
  return mrb_realloc_simple(mrb, 0, len);
}
