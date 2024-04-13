static int ReadBlobLZWByte(LZWInfo *lzw_info)
{
  int
    code;

  size_t
    one,
    value;

  ssize_t
    count;

  if (lzw_info->stack->index != lzw_info->stack->codes)
    return(PopLZWStack(lzw_info->stack));
  if (lzw_info->genesis != MagickFalse)
    {
      lzw_info->genesis=MagickFalse;
      do
      {
        lzw_info->first_code=(size_t) GetNextLZWCode(lzw_info,lzw_info->bits);
        lzw_info->last_code=lzw_info->first_code;
      } while (lzw_info->first_code == lzw_info->clear_code);
      return((int) lzw_info->first_code);
    }
  code=GetNextLZWCode(lzw_info,lzw_info->bits);
  if (code < 0)
    return(code);
  if ((size_t) code == lzw_info->clear_code)
    {
      ResetLZWInfo(lzw_info);
      return(ReadBlobLZWByte(lzw_info));
    }
  if ((size_t) code == lzw_info->end_code)
    return(-1);
  if ((size_t) code < lzw_info->slot)
    value=(size_t) code;
  else
    {
      PushLZWStack(lzw_info->stack,lzw_info->first_code);
      value=lzw_info->last_code;
    }
  count=0;
  while (value > lzw_info->maximum_data_value)
  {
    if ((size_t) count > MaximumLZWCode)
      return(-1);
    count++;
    if ((size_t) value > MaximumLZWCode)
      return(-1);
    PushLZWStack(lzw_info->stack,lzw_info->table[1][value]);
    value=lzw_info->table[0][value];
  }
  lzw_info->first_code=lzw_info->table[1][value];
  PushLZWStack(lzw_info->stack,lzw_info->first_code);
  one=1;
  if (lzw_info->slot < MaximumLZWCode)
    {
      lzw_info->table[0][lzw_info->slot]=lzw_info->last_code;
      lzw_info->table[1][lzw_info->slot]=lzw_info->first_code;
      lzw_info->slot++;
      if ((lzw_info->slot >= lzw_info->maximum_code) &&
          (lzw_info->bits < MaximumLZWBits))
        {
          lzw_info->bits++;
          lzw_info->maximum_code=one << lzw_info->bits;
        }
    }
  lzw_info->last_code=(size_t) code;
  return(PopLZWStack(lzw_info->stack));
}
