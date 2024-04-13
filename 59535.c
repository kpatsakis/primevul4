static MagickBooleanType IsTIFF(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"\115\115\000\052",4) == 0)
    return(MagickTrue);
  if (memcmp(magick,"\111\111\052\000",4) == 0)
    return(MagickTrue);
#if defined(TIFF_VERSION_BIG)
  if (length < 8)
    return(MagickFalse);
  if (memcmp(magick,"\115\115\000\053\000\010\000\000",8) == 0)
    return(MagickTrue);
  if (memcmp(magick,"\111\111\053\000\010\000\000\000",8) == 0)
    return(MagickTrue);
#endif
  return(MagickFalse);
}
