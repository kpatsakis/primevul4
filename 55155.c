static inline void ResetLZWInfo(LZWInfo *lzw_info)
{
  size_t
    one;

  lzw_info->bits=lzw_info->data_size+1;
  one=1;
  lzw_info->maximum_code=one << lzw_info->bits;
  lzw_info->slot=lzw_info->maximum_data_value+3;
  lzw_info->genesis=MagickTrue;
}
