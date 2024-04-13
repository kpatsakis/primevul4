static inline int PopLZWStack(LZWStack *stack_info)
{
  if (stack_info->index <= stack_info->codes)
    return(-1);
  stack_info->index--;
  return((int) *stack_info->index);
}
