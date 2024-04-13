MagickExport MagickBooleanType SetImageClipMask(Image *image,
  const Image *clip_mask)
{
  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickSignature);
  if (clip_mask != (const Image *) NULL)
    if ((clip_mask->columns != image->columns) ||
        (clip_mask->rows != image->rows))
      ThrowBinaryException(ImageError,"ImageSizeDiffers",image->filename);
  if (image->clip_mask != (Image *) NULL)
    image->clip_mask=DestroyImage(image->clip_mask);
  image->clip_mask=NewImageList();
  if (clip_mask == (Image *) NULL)
    return(MagickTrue);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    return(MagickFalse);
  image->clip_mask=CloneImage(clip_mask,0,0,MagickTrue,&image->exception);
  if (image->clip_mask == (Image *) NULL)
    return(MagickFalse);
  return(MagickTrue);
}
