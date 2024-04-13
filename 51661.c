static MagickBooleanType IsDCM(const unsigned char *magick,const size_t length)
{
  if (length < 132)
    return(MagickFalse);
  if (LocaleNCompare((char *) (magick+128),"DICM",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
