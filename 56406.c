static MagickBooleanType IsPDF(const unsigned char *magick,const size_t offset)
{
  if (offset < 5)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick,"%PDF-",5) == 0)
    return(MagickTrue);
  return(MagickFalse);
}
