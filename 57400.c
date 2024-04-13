MagickExport Image *GetImageClipMask(const Image *image,
  ExceptionInfo *exception)
{
  assert(image != (const Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickSignature);
  if (image->clip_mask == (Image *) NULL)
    return((Image *) NULL);
  return(CloneImage(image->clip_mask,0,0,MagickTrue,exception));
}
