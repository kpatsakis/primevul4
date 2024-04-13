MagickExport Image *ReferenceImage(Image *image)
{
  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickSignature);
  LockSemaphoreInfo(image->semaphore);
  image->reference_count++;
  UnlockSemaphoreInfo(image->semaphore);
  return(image);
}
