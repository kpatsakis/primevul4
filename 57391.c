MagickExport void AcquireNextImage(const ImageInfo *image_info,Image *image)
{
  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  image->next=AcquireImage(image_info);
  if (GetNextImageInList(image) == (Image *) NULL)
    return;
  (void) CopyMagickString(GetNextImageInList(image)->filename,image->filename,
    MaxTextExtent);
  if (image_info != (ImageInfo *) NULL)
    (void) CopyMagickString(GetNextImageInList(image)->filename,
      image_info->filename,MaxTextExtent);
  DestroyBlob(GetNextImageInList(image));
  image->next->blob=ReferenceBlob(image->blob);
  image->next->endian=image->endian;
  image->next->scene=image->scene+1;
  image->next->previous=image;
}
