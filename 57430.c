MagickExport MagickBooleanType SyncImagesSettings(ImageInfo *image_info,
  Image *images)
{
  Image
    *image;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(images != (Image *) NULL);
  assert(images->signature == MagickSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  image=images;
  for ( ; image != (Image *) NULL; image=GetNextImageInList(image))
    (void) SyncImageSettings(image_info,image);
  (void) DeleteImageOption(image_info,"page");
  return(MagickTrue);
}
