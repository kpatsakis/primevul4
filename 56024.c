static MagickBooleanType IsJP2(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"\x0d\x0a\x87\x0a",4) == 0)
    return(MagickTrue);
  if (length < 12)
    return(MagickFalse);
  if (memcmp(magick,"\x00\x00\x00\x0c\x6a\x50\x20\x20\x0d\x0a\x87\x0a",12) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
