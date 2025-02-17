static inline void SetPSDPixel(Image *image,const size_t channels,
  const ssize_t type,const size_t packet_size,const Quantum pixel,
  PixelPacket *q,IndexPacket *indexes,ssize_t x)
{
  if (image->storage_class == PseudoClass)
    {
      if (packet_size == 1)
        SetPixelIndex(indexes+x,ScaleQuantumToChar(pixel));
      else
        SetPixelIndex(indexes+x,ScaleQuantumToShort(pixel));
      SetPixelRGBO(q,image->colormap+(ssize_t)
        ConstrainColormapIndex(image,GetPixelIndex(indexes+x)));
      return;
    }
  switch (type)
  {
    case -1:
    {
      SetPixelAlpha(q,pixel);
      break;
    }
    case -2:
    case 0:
    {
      SetPixelRed(q,pixel);
      if (channels == 1 || type == -2)
        {
          SetPixelGreen(q,GetPixelRed(q));
          SetPixelBlue(q,GetPixelRed(q));
        }
      break;
    }
    case 1:
    {
      if (image->storage_class == PseudoClass)
        SetPixelAlpha(q,pixel);
      else
        SetPixelGreen(q,pixel);
      break;
    }
    case 2:
    {
      if (image->storage_class == PseudoClass)
        SetPixelAlpha(q,pixel);
      else
        SetPixelBlue(q,pixel);
      break;
    }
    case 3:
    {
      if (image->colorspace == CMYKColorspace)
        SetPixelIndex(indexes+x,pixel);
      else
        if (image->matte != MagickFalse)
          SetPixelAlpha(q,pixel);
      break;
    }
    case 4:
    {
      if ((IssRGBCompatibleColorspace(image->colorspace) != MagickFalse) &&
          (channels > 3))
        break;
      if (image->matte != MagickFalse)
        SetPixelAlpha(q,pixel);
      break;
    }
  }
}
