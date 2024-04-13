static MagickBooleanType IsJ2K(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"\xff\x4f\xff\x51",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
