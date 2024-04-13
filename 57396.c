MagickExport Image *DestroyImage(Image *image)
{
  MagickBooleanType
    destroy;

  /*
    Dereference image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  destroy=MagickFalse;
  LockSemaphoreInfo(image->semaphore);
  image->reference_count--;
  if (image->reference_count == 0)
    destroy=MagickTrue;
  UnlockSemaphoreInfo(image->semaphore);
  if (destroy == MagickFalse)
    return((Image *) NULL);
  /*
    Destroy image.
  */
  DestroyImagePixels(image);
  if (image->clip_mask != (Image *) NULL)
    image->clip_mask=DestroyImage(image->clip_mask);
  if (image->mask != (Image *) NULL)
    image->mask=DestroyImage(image->mask);
  if (image->montage != (char *) NULL)
    image->montage=DestroyString(image->montage);
  if (image->directory != (char *) NULL)
    image->directory=DestroyString(image->directory);
  if (image->colormap != (PixelPacket *) NULL)
    image->colormap=(PixelPacket *) RelinquishMagickMemory(image->colormap);
  if (image->geometry != (char *) NULL)
    image->geometry=DestroyString(image->geometry);
  DestroyImageProfiles(image);
  DestroyImageProperties(image);
  DestroyImageArtifacts(image);
  if (image->ascii85 != (Ascii85Info*) NULL)
    image->ascii85=(Ascii85Info *) RelinquishMagickMemory(image->ascii85);
  DestroyBlob(image);
  (void) ClearExceptionInfo(&image->exception,MagickTrue);
  if (image->semaphore != (SemaphoreInfo *) NULL)
    DestroySemaphoreInfo(&image->semaphore);
  image->signature=(~MagickSignature);
  image=(Image *) RelinquishMagickMemory(image);
  return(image);
}
