MagickExport MagickBooleanType SetImageStorageClass(Image *image,
  const ClassType storage_class)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  image->storage_class=storage_class;
  return(SyncImagePixelCache(image,&image->exception));
}
