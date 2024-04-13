static LZWInfo *RelinquishLZWInfo(LZWInfo *lzw_info)
{
  if (lzw_info->table[0] != (size_t *) NULL)
    lzw_info->table[0]=(size_t *) RelinquishMagickMemory(
      lzw_info->table[0]);
  if (lzw_info->table[1] != (size_t *) NULL)
    lzw_info->table[1]=(size_t *) RelinquishMagickMemory(
      lzw_info->table[1]);
  if (lzw_info->stack != (LZWStack *) NULL)
    {
      if (lzw_info->stack->codes != (size_t *) NULL)
        lzw_info->stack->codes=(size_t *) RelinquishMagickMemory(
          lzw_info->stack->codes);
      lzw_info->stack=(LZWStack *) RelinquishMagickMemory(lzw_info->stack);
    }
  lzw_info=(LZWInfo *) RelinquishMagickMemory(lzw_info);
  return((LZWInfo *) NULL);
}
