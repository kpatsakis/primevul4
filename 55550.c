static MagickBooleanType ApplyPSDOpacityMask(Image *image,const Image *mask,
  Quantum background,MagickBooleanType revert,ExceptionInfo *exception)
{
  Image
    *complete_mask;

  MagickBooleanType
    status;

  MagickPixelPacket
    color;

  ssize_t
    y;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  applying opacity mask");
  complete_mask=CloneImage(image,image->columns,image->rows,MagickTrue,
    exception);
  complete_mask->matte=MagickTrue;
  GetMagickPixelPacket(complete_mask,&color);
  color.red=background;
  SetImageColor(complete_mask,&color);
  status=CompositeImage(complete_mask,OverCompositeOp,mask,
    mask->page.x-image->page.x,mask->page.y-image->page.y);
  if (status == MagickFalse)
    {
      complete_mask=DestroyImage(complete_mask);
      return(status);
    }
  image->matte=MagickTrue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#pragma omp parallel for schedule(static,4) shared(status) \
  magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register PixelPacket
      *magick_restrict q;

    register PixelPacket
      *p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    p=GetAuthenticPixels(complete_mask,0,y,complete_mask->columns,1,exception);
    if ((q == (PixelPacket *) NULL) || (p == (PixelPacket *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      MagickRealType
        alpha,
        intensity;

      alpha=GetPixelAlpha(q);
      intensity=GetPixelIntensity(complete_mask,p);
      if (revert == MagickFalse)
        SetPixelAlpha(q,ClampToQuantum(intensity*(QuantumScale*alpha)));
      else if (intensity > 0)
        SetPixelAlpha(q,ClampToQuantum((alpha/intensity)*QuantumRange));
      q++;
      p++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      status=MagickFalse;
  }
  complete_mask=DestroyImage(complete_mask);
  return(status);
}
