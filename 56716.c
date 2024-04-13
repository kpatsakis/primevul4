static inline MagickBooleanType Magick_png_color_equal(const Image *image,
  const Quantum *p, const PixelInfo *q)
{
  MagickRealType
    value;

  value=(MagickRealType) p[image->channel_map[RedPixelChannel].offset];
  if (AbsolutePixelValue(value-q->red) >= MagickEpsilon)
    return(MagickFalse);
  value=(MagickRealType) p[image->channel_map[GreenPixelChannel].offset];
  if (AbsolutePixelValue(value-q->green) >= MagickEpsilon)
    return(MagickFalse);
  value=(MagickRealType) p[image->channel_map[BluePixelChannel].offset];
  if (AbsolutePixelValue(value-q->blue) >= MagickEpsilon)
    return(MagickFalse);

  return(MagickTrue);
}
