static MagickBooleanType ReadPSDLayer(Image *image,const ImageInfo *image_info,
  const PSDInfo *psd_info,LayerInfo* layer_info,ExceptionInfo *exception)
{
  char
    message[MagickPathExtent];

  MagickBooleanType
    status;

  PSDCompressionType
    compression;

  ssize_t
    j;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "    setting up new layer image");
  if (psd_info->mode != IndexedMode)
    (void) SetImageBackgroundColor(layer_info->image,exception);
  layer_info->image->compose=PSDBlendModeToCompositeOperator(
    layer_info->blendkey);
  if (layer_info->visible == MagickFalse)
    layer_info->image->compose=NoCompositeOp;
  if (psd_info->mode == CMYKMode)
    SetImageColorspace(layer_info->image,CMYKColorspace,exception);
  else if ((psd_info->mode == BitmapMode) || (psd_info->mode == DuotoneMode) ||
           (psd_info->mode == GrayscaleMode))
    SetImageColorspace(layer_info->image,GRAYColorspace,exception);
  /*
    Set up some hidden attributes for folks that need them.
  */
  (void) FormatLocaleString(message,MagickPathExtent,"%.20g",
    (double) layer_info->page.x);
  (void) SetImageArtifact(layer_info->image,"psd:layer.x",message);
  (void) FormatLocaleString(message,MagickPathExtent,"%.20g",
    (double) layer_info->page.y);
  (void) SetImageArtifact(layer_info->image,"psd:layer.y",message);
  (void) FormatLocaleString(message,MagickPathExtent,"%.20g",(double)
    layer_info->opacity);
  (void) SetImageArtifact(layer_info->image,"psd:layer.opacity",message);
  (void) SetImageProperty(layer_info->image,"label",(char *) layer_info->name,
    exception);

  status=MagickTrue;
  for (j=0; j < (ssize_t) layer_info->channels; j++)
  {
    if (image->debug != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    reading data for channel %.20g",(double) j);

    compression=(PSDCompressionType) ReadBlobShort(layer_info->image);
    layer_info->image->compression=ConvertPSDCompression(compression);
    if (layer_info->channel_info[j].type == -1)
      layer_info->image->alpha_trait=BlendPixelTrait;

    status=ReadPSDChannel(layer_info->image,image_info,psd_info,layer_info,j,
      compression,exception);

    if (status == MagickFalse)
      break;
  }

  if (status != MagickFalse)
    status=ApplyPSDLayerOpacity(layer_info->image,layer_info->opacity,
      MagickFalse,exception);

  if ((status != MagickFalse) &&
      (layer_info->image->colorspace == CMYKColorspace))
    status=NegateCMYK(layer_info->image,exception);

  if ((status != MagickFalse) && (layer_info->mask.image != (Image *) NULL))
    {
      const char
        *option;
      
      layer_info->mask.image->page.x=layer_info->mask.page.x;
      layer_info->mask.image->page.y=layer_info->mask.page.y;
      /* Do not composite the mask when it is disabled */
      if ((layer_info->mask.flags & 0x02) == 0x02)
        layer_info->mask.image->compose=NoCompositeOp;
      else
        status=ApplyPSDOpacityMask(layer_info->image,layer_info->mask.image,
          layer_info->mask.background == 0 ? 0 : QuantumRange,MagickFalse,
          exception);
      option=GetImageOption(image_info,"psd:preserve-opacity-mask");
      if (IsStringTrue(option) != MagickFalse)
        PreservePSDOpacityMask(image,layer_info,exception);
      layer_info->mask.image=DestroyImage(layer_info->mask.image);
    }

  return(status);
}
