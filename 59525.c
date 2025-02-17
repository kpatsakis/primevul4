static inline void SetPSDPixel(Image *image,const size_t channels,
  const ssize_t type,const size_t packet_size,const Quantum pixel,Quantum *q,
  ExceptionInfo *exception)
{
  if (image->storage_class == PseudoClass)
    {
      if (packet_size == 1)
        SetPixelIndex(image,ScaleQuantumToChar(pixel),q);
      else
        SetPixelIndex(image,ScaleQuantumToShort(pixel),q);
      SetPixelViaPixelInfo(image,image->colormap+(ssize_t)
        ConstrainColormapIndex(image,GetPixelIndex(image,q),exception),q);
      return;
    }
  switch (type)
  {
    case -1:
    {
      SetPixelAlpha(image, pixel,q);
      break;
    }
    case -2:
    case 0:
    {
      SetPixelRed(image,pixel,q);
      if (channels == 1 || type == -2)
        SetPixelGray(image,pixel,q);
      break;
    }
    case 1:
    {
      if (image->storage_class == PseudoClass)
        SetPixelAlpha(image,pixel,q);
      else
        SetPixelGreen(image,pixel,q);
      break;
    }
    case 2:
    {
      if (image->storage_class == PseudoClass)
        SetPixelAlpha(image,pixel,q);
      else
        SetPixelBlue(image,pixel,q);
      break;
    }
    case 3:
    {
      if (image->colorspace == CMYKColorspace)
        SetPixelBlack(image,pixel,q);
      else
        if (image->alpha_trait != UndefinedPixelTrait)
          SetPixelAlpha(image,pixel,q);
      break;
    }
    case 4:
    {
      if ((IssRGBCompatibleColorspace(image->colorspace) != MagickFalse) &&
          (channels > 3))
        break;
      if (image->alpha_trait != UndefinedPixelTrait)
        SetPixelAlpha(image,pixel,q);
      break;
    }
  }
}
