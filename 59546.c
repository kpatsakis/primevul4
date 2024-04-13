static void TIFFReadPhotoshopLayers(Image* image,const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  const char
    *option;

  const StringInfo
    *layer_info;

  Image
    *layers;

  PSDInfo
    info;

  register ssize_t
    i;

  if (GetImageListLength(image) != 1)
    return;
  if ((image_info->number_scenes == 1) && (image_info->scene == 0))
    return;
  option=GetImageOption(image_info,"tiff:ignore-layers");
  if (option != (const char * ) NULL)
    return;
  layer_info=GetImageProfile(image,"tiff:37724");
  if (layer_info == (const StringInfo *) NULL)
    return;
  for (i=0; i < (ssize_t) layer_info->length-8; i++)
  {
    if (LocaleNCompare((const char *) (layer_info->datum+i),
        image->endian == MSBEndian ? "8BIM" : "MIB8",4) != 0)
      continue;
    i+=4;
    if ((LocaleNCompare((const char *) (layer_info->datum+i),
         image->endian == MSBEndian ? "Layr" : "ryaL",4) == 0) ||
        (LocaleNCompare((const char *) (layer_info->datum+i),
         image->endian == MSBEndian ? "LMsk" : "ksML",4) == 0) ||
        (LocaleNCompare((const char *) (layer_info->datum+i),
         image->endian == MSBEndian ? "Lr16" : "61rL",4) == 0) ||
        (LocaleNCompare((const char *) (layer_info->datum+i),
         image->endian == MSBEndian ? "Lr32" : "23rL",4) == 0))
      break;
  }
  i+=4;
  if (i >= (ssize_t) (layer_info->length-8))
    return;
  layers=CloneImage(image,image->columns,image->rows,MagickTrue,exception);
  (void) DeleteImageProfile(layers,"tiff:37724");
  AttachBlob(layers->blob,layer_info->datum,layer_info->length);
  SeekBlob(layers,(MagickOffsetType) i,SEEK_SET);
  info.version=1;
  info.columns=layers->columns;
  info.rows=layers->rows;
  /* Setting the mode to a value that won't change the colorspace */
  info.mode=10;
  if (IsGrayImage(image,&image->exception) != MagickFalse)
    info.channels=(image->matte != MagickFalse ? 2UL : 1UL);
  else
    if (image->storage_class == PseudoClass)
      info.channels=(image->matte != MagickFalse ? 2UL : 1UL);
    else
      {
        if (image->colorspace != CMYKColorspace)
          info.channels=(image->matte != MagickFalse ? 4UL : 3UL);
        else
          info.channels=(image->matte != MagickFalse ? 5UL : 4UL);
      }
  (void) ReadPSDLayers(layers,image_info,&info,MagickFalse,exception);
  InheritException(exception,&layers->exception);
  DeleteImageFromList(&layers);
  if (layers != (Image *) NULL)
    {
      SetImageArtifact(image,"tiff:has-layers","true");
      AppendImageToList(&image,layers);
      while (layers != (Image *) NULL)
      {
        SetImageArtifact(layers,"tiff:has-layers","true");
        DetachBlob(layers->blob);
        layers=GetNextImageInList(layers);
      }
    }
}
