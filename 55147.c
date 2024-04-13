static MagickBooleanType IsGIF(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (LocaleNCompare((char *) magick,"GIF8",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
