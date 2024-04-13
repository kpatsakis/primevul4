stack_copy(mrb_value *dst, const mrb_value *src, size_t size)
{
  memcpy(dst, src, sizeof(mrb_value)*size);
}