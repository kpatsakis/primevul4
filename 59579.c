MagickExport void ResetImageProfileIterator(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    return;
  ResetSplayTreeIterator((SplayTreeInfo *) image->profiles);
}
