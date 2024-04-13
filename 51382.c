MagickExport const char *GetNextImageProperty(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image->filename);
  if (image->properties == (void *) NULL)
    return((const char *) NULL);
  return((const char *) GetNextKeyInSplayTree((SplayTreeInfo *) image->properties));
}
