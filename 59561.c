MagickExport MagickBooleanType CloneImageProfiles(Image *image,
  const Image *clone_image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(clone_image != (const Image *) NULL);
  assert(clone_image->signature == MagickSignature);
  image->color_profile.length=clone_image->color_profile.length;
  image->color_profile.info=clone_image->color_profile.info;
  image->iptc_profile.length=clone_image->iptc_profile.length;
  image->iptc_profile.info=clone_image->iptc_profile.info;
  if (clone_image->profiles != (void *) NULL)
    {
      if (image->profiles != (void *) NULL)
        DestroyImageProfiles(image);
      image->profiles=CloneSplayTree((SplayTreeInfo *) clone_image->profiles,
        (void *(*)(void *)) ConstantString,(void *(*)(void *)) CloneStringInfo);
   }
  return(MagickTrue);
}
