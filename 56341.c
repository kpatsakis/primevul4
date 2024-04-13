static MagickBooleanType IsCALS(const unsigned char *magick,const size_t length)
{
  if (length < 128)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick,"version: MIL-STD-1840",21) == 0)
    return(MagickTrue);
  if (LocaleNCompare((const char *) magick,"srcdocid:",9) == 0)
    return(MagickTrue);
  if (LocaleNCompare((const char *) magick,"rorient:",8) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
