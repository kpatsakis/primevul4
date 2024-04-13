stack_clear(mrb_value *from, size_t count)
{
  while (count-- > 0) {
    SET_NIL_VALUE(*from);
    from++;
  }
}