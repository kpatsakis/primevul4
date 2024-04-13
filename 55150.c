static inline void PushLZWStack(LZWStack *stack_info,const size_t value)
{
  if (stack_info->index >= stack_info->top)
    return;
  *stack_info->index=value;
  stack_info->index++;
}
