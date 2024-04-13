MagickExport MagickBooleanType IsHighDynamicRangeImage(const Image *image,
  ExceptionInfo *exception)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  (void) image;
  (void) exception;
  return(MagickFalse);
#else
  CacheView
    *image_view;

  MagickBooleanType
    status;

  MagickPixelPacket
    zero;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=MagickTrue;
  GetMagickPixelPacket(image,&zero);
  image_view=AcquireVirtualCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    MagickPixelPacket
      pixel;

    register const IndexPacket
      *indexes;

    register const PixelPacket
      *p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewVirtualIndexQueue(image_view);
    pixel=zero;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetMagickPixelPacket(image,p,indexes+x,&pixel);
      if ((pixel.red < 0.0) || (pixel.red > QuantumRange) ||
          (pixel.red != (QuantumAny) pixel.red))
        break;
      if ((pixel.green < 0.0) || (pixel.green > QuantumRange) ||
          (pixel.green != (QuantumAny) pixel.green))
        break;
      if ((pixel.blue < 0.0) || (pixel.blue > QuantumRange) ||
          (pixel.blue != (QuantumAny) pixel.blue))
        break;
      if (pixel.matte != MagickFalse)
        {
          if ((pixel.opacity < 0.0) || (pixel.opacity > QuantumRange) ||
              (pixel.opacity != (QuantumAny) pixel.opacity))
            break;
        }
      if (pixel.colorspace == CMYKColorspace)
        {
          if ((pixel.index < 0.0) || (pixel.index > QuantumRange) ||
              (pixel.index != (QuantumAny) pixel.index))
            break;
        }
      p++;
    }
    if (x < (ssize_t) image->columns)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  return(status != MagickFalse ? MagickFalse : MagickTrue);
#endif
}
