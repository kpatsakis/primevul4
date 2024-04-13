MagickExport MagickBooleanType SetImageProfile(Image *image,const char *name,
  const StringInfo *profile)
{
  return(SetImageProfileInternal(image,name,profile,MagickFalse));
}
