MagickExport const StringInfo *GetImageProfile(const Image *image,
  const char *name)
{
  const StringInfo
    *profile;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    return((StringInfo *) NULL);
  profile=(const StringInfo *) GetValueFromSplayTree((SplayTreeInfo *)
    image->profiles,name);
  return(profile);
}
