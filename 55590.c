static MagickBooleanType IsPS(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"%!",2) == 0)
    return(MagickTrue);
  if (memcmp(magick,"\004%!",3) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
