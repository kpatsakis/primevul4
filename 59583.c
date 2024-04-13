MagickExport MagickBooleanType SyncImageProfiles(Image *image)
{
  MagickBooleanType
    status;

  StringInfo
    *profile;

  status=MagickTrue;
  profile=(StringInfo *) GetImageProfile(image,"8BIM");
  if (profile != (StringInfo *) NULL)
    if (Sync8BimProfile(image,profile) == MagickFalse)
      status=MagickFalse;
  profile=(StringInfo *) GetImageProfile(image,"EXIF");
  if (profile != (StringInfo *) NULL)
    if (SyncExifProfile(image,profile) == MagickFalse)
      status=MagickFalse;
  return(status);
}
