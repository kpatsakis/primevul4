MagickExport MagickBooleanType CopyImagePixels(Image *image,
  const Image *source_image,const RectangleInfo *geometry,
  const OffsetInfo *offset,ExceptionInfo *exception)
{
#define CopyImageTag  "Copy/Image"

  CacheView
    *image_view,
    *source_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(source_image != (Image *) NULL);
  assert(geometry != (RectangleInfo *) NULL);
  assert(offset != (OffsetInfo *) NULL);
  if ((offset->x < 0) || (offset->y < 0) ||
      ((ssize_t) (offset->x+geometry->width) > (ssize_t) image->columns) ||
      ((ssize_t) (offset->y+geometry->height) > (ssize_t) image->rows))
    ThrowBinaryException(OptionError,"GeometryDoesNotContainImage",
      image->filename);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    return(MagickFalse);
  /*
    Copy image pixels.
  */
  status=MagickTrue;
  progress=0;
  source_view=AcquireVirtualCacheView(source_image,exception);
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(progress,status) \
    magick_threads(source_image,image,geometry->height,1)
#endif
  for (y=0; y < (ssize_t) geometry->height; y++)
  {
    register const IndexPacket
      *magick_restrict source_indexes;

    register const PixelPacket
      *magick_restrict p;

    register IndexPacket
      *magick_restrict indexes;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(source_view,geometry->x,y+geometry->y,
      geometry->width,1,exception);
    q=GetCacheViewAuthenticPixels(image_view,offset->x,y+offset->y,
      geometry->width,1,exception);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    source_indexes=GetCacheViewVirtualIndexQueue(source_view);
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) geometry->width; x++)
    {
      *q=(*p);
      if (image->colorspace == CMYKColorspace)
        indexes[x]=source_indexes[x];
      p++;
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_CopyImagePixels)
#endif
        proceed=SetImageProgress(image,CopyImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  source_view=DestroyCacheView(source_view);
  return(status);
}
