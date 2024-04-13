static MagickBooleanType IsSUN(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"\131\246\152\225",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
