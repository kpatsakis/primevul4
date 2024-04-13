static inline int GetNextLZWCode(LZWInfo *lzw_info,const size_t bits)
{
  int
    code;

  register ssize_t
    i;

  size_t
    one;

  while (((lzw_info->code_info.bit+bits) > (8*lzw_info->code_info.count)) &&
         (lzw_info->code_info.eof == MagickFalse))
  {
    ssize_t
      count;

    lzw_info->code_info.buffer[0]=lzw_info->code_info.buffer[
      lzw_info->code_info.count-2];
    lzw_info->code_info.buffer[1]=lzw_info->code_info.buffer[
      lzw_info->code_info.count-1];
    lzw_info->code_info.bit-=8*(lzw_info->code_info.count-2);
    lzw_info->code_info.count=2;
    count=ReadBlobBlock(lzw_info->image,&lzw_info->code_info.buffer[
      lzw_info->code_info.count]);
    if (count > 0)
      lzw_info->code_info.count+=count;
    else
      lzw_info->code_info.eof=MagickTrue;
  }
  if ((lzw_info->code_info.bit+bits) > (8*lzw_info->code_info.count))
    return(-1);
  code=0;
  one=1;
  for (i=0; i < (ssize_t) bits; i++)
  {
    code|=((lzw_info->code_info.buffer[lzw_info->code_info.bit/8] &
      (one << (lzw_info->code_info.bit % 8))) != 0) << i;
    lzw_info->code_info.bit++;
  }
  return(code);
}
