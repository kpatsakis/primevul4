static MagickBooleanType IsPCX(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(MagickFalse);
  if (memcmp(magick,"\012\002",2) == 0)
    return(MagickTrue);
  if (memcmp(magick,"\012\005",2) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
