static MagickBooleanType CorrectPSDAlphaBlend(const ImageInfo *image_info,
  Image *image, ExceptionInfo* exception)
{
  const char
    *option;

  MagickBooleanType
    status;

  ssize_t
    y;

  if (image->matte == MagickFalse || image->colorspace != sRGBColorspace)
    return(MagickTrue);
  option=GetImageOption(image_info,"psd:alpha-unblend");
  if (IsStringNotFalse(option) == MagickFalse)
    return(MagickTrue);
  status=MagickTrue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#pragma omp parallel for schedule(static,4) shared(status) \
  magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      double
        gamma;

      gamma=QuantumScale*GetPixelAlpha(q);
      if (gamma != 0.0 && gamma != 1.0)
        {
          SetPixelRed(q,(GetPixelRed(q)-((1.0-gamma)*QuantumRange))/gamma);
          SetPixelGreen(q,(GetPixelGreen(q)-((1.0-gamma)*QuantumRange))/gamma);
          SetPixelBlue(q,(GetPixelBlue(q)-((1.0-gamma)*QuantumRange))/gamma);
        }
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      status=MagickFalse;
  }

  return(status);
}
