MagickExport VirtualPixelMethod GetImageVirtualPixelMethod(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  return(GetPixelCacheVirtualMethod(image));
}
