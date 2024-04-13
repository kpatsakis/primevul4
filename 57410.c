MagickExport MagickBooleanType ModifyImage(Image **image,
  ExceptionInfo *exception)
{
  Image
    *clone_image;

  assert(image != (Image **) NULL);
  assert(*image != (Image *) NULL);
  assert((*image)->signature == MagickSignature);
  if ((*image)->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",(*image)->filename);
  if (GetImageReferenceCount(*image) <= 1)
    return(MagickTrue);
  clone_image=CloneImage(*image,0,0,MagickTrue,exception);
  LockSemaphoreInfo((*image)->semaphore);
  (*image)->reference_count--;
  UnlockSemaphoreInfo((*image)->semaphore);
  *image=clone_image;
  return(MagickTrue);
}
