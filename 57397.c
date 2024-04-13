MagickExport ImageInfo *DestroyImageInfo(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->size != (char *) NULL)
    image_info->size=DestroyString(image_info->size);
  if (image_info->extract != (char *) NULL)
    image_info->extract=DestroyString(image_info->extract);
  if (image_info->scenes != (char *) NULL)
    image_info->scenes=DestroyString(image_info->scenes);
  if (image_info->page != (char *) NULL)
    image_info->page=DestroyString(image_info->page);
  if (image_info->sampling_factor != (char *) NULL)
    image_info->sampling_factor=DestroyString(
      image_info->sampling_factor);
  if (image_info->server_name != (char *) NULL)
    image_info->server_name=DestroyString(
      image_info->server_name);
  if (image_info->font != (char *) NULL)
    image_info->font=DestroyString(image_info->font);
  if (image_info->texture != (char *) NULL)
    image_info->texture=DestroyString(image_info->texture);
  if (image_info->density != (char *) NULL)
    image_info->density=DestroyString(image_info->density);
  if (image_info->view != (char *) NULL)
    image_info->view=DestroyString(image_info->view);
  if (image_info->authenticate != (char *) NULL)
    image_info->authenticate=DestroyString(
      image_info->authenticate);
  DestroyImageOptions(image_info);
  if (image_info->cache != (void *) NULL)
    image_info->cache=DestroyPixelCache(image_info->cache);
  if (image_info->profile != (StringInfo *) NULL)
    image_info->profile=(void *) DestroyStringInfo((StringInfo *)
      image_info->profile);
  image_info->signature=(~MagickSignature);
  image_info=(ImageInfo *) RelinquishMagickMemory(image_info);
  return(image_info);
}
