static MagickBooleanType IsDCX(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"\261\150\336\72",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
