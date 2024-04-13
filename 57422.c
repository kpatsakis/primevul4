MagickExport MagickBooleanType SetImageMask(Image *image,const Image *mask)
{
  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickSignature);
  if (mask != (const Image *) NULL)
    if ((mask->columns != image->columns) || (mask->rows != image->rows))
      ThrowBinaryException(ImageError,"ImageSizeDiffers",image->filename);
  if (image->mask != (Image *) NULL)
    image->mask=DestroyImage(image->mask);
  image->mask=NewImageList();
  if (mask == (Image *) NULL)
    return(MagickTrue);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    return(MagickFalse);
  image->mask=CloneImage(mask,0,0,MagickTrue,&image->exception);
  if (image->mask == (Image *) NULL)
    return(MagickFalse);
  return(MagickTrue);
}
