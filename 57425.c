MagickExport VirtualPixelMethod SetImageVirtualPixelMethod(const Image *image,
  const VirtualPixelMethod virtual_pixel_method)
{
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  return(SetPixelCacheVirtualMethod(image,virtual_pixel_method));
}
