MagickExport MagickBooleanType QuantizeImage(const QuantizeInfo *quantize_info,
  Image *image)
{
  CubeInfo
    *cube_info;

  MagickBooleanType
    status;

  size_t
    depth,
    maximum_colors;

  assert(quantize_info != (const QuantizeInfo *) NULL);
  assert(quantize_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  maximum_colors=quantize_info->number_colors;
  if (maximum_colors == 0)
    maximum_colors=MaxColormapSize;
  if (maximum_colors > MaxColormapSize)
    maximum_colors=MaxColormapSize;
  if (image->matte == MagickFalse)
    {
      if (SetImageGray(image,&image->exception) != MagickFalse)
        (void) SetGrayscaleImage(image);
    }
  if ((image->storage_class == PseudoClass) &&
      (image->colors <= maximum_colors))
    {
      if ((quantize_info->colorspace != UndefinedColorspace) &&
          (quantize_info->colorspace != CMYKColorspace))
        (void) TransformImageColorspace(image,quantize_info->colorspace);
      return(MagickTrue);
    }
  depth=quantize_info->tree_depth;
  if (depth == 0)
    {
      size_t
        colors;

      /*
        Depth of color tree is: Log4(colormap size)+2.
      */
      colors=maximum_colors;
      for (depth=1; colors != 0; depth++)
        colors>>=2;
      if ((quantize_info->dither != MagickFalse) && (depth > 2))
        depth--;
      if ((image->matte != MagickFalse) && (depth > 5))
        depth--;
      if (SetImageGray(image,&image->exception) != MagickFalse)
        depth=MaxTreeDepth;
    }
  /*
    Initialize color cube.
  */
  cube_info=GetCubeInfo(quantize_info,depth,maximum_colors);
  if (cube_info == (CubeInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  status=ClassifyImageColors(cube_info,image,&image->exception);
  if (status != MagickFalse)
    {
      /*
        Reduce the number of colors in the image if it contains more than the
        maximum, otherwise we can disable dithering to improve the performance.
      */
      if (cube_info->colors > cube_info->maximum_colors)
        ReduceImageColors(image,cube_info);
      else
        cube_info->quantize_info->dither_method=NoDitherMethod;
      status=AssignImageColors(image,cube_info);
    }
  DestroyCubeInfo(cube_info);
  return(status);
}
