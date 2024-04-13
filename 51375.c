MagickExport void DestroyImageProperties(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->properties != (void *) NULL)
    image->properties=(void *) DestroySplayTree((SplayTreeInfo *)
      image->properties);
}
