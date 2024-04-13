MagickExport Image *NewMagickImage(const ImageInfo *image_info,
  const size_t width,const size_t height,const MagickPixelPacket *background)
{
  CacheView
    *image_view;

  ExceptionInfo
    *exception;

  Image
    *image;

  ssize_t
    y;

  MagickBooleanType
    status;

  assert(image_info != (const ImageInfo *) NULL);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image_info->signature == MagickSignature);
  assert(background != (const MagickPixelPacket *) NULL);
  image=AcquireImage(image_info);
  image->columns=width;
  image->rows=height;
  image->colorspace=background->colorspace;
  image->matte=background->matte;
  image->fuzz=background->fuzz;
  image->depth=background->depth;
  status=MagickTrue;
  exception=(&image->exception);
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register IndexPacket
      *magick_restrict indexes;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelPacket(image,background,q,indexes+x);
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    image=DestroyImage(image);
  return(image);
}
