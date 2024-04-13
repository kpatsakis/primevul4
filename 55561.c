static void PreservePSDOpacityMask(Image *image,LayerInfo* layer_info,
  ExceptionInfo *exception)
{
  char
    *key;

  RandomInfo
    *random_info;

  StringInfo
    *key_info;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  preserving opacity mask");
  random_info=AcquireRandomInfo();
  key_info=GetRandomKey(random_info,2+1);
  key=(char *) GetStringInfoDatum(key_info);
  key[8]=layer_info->mask.background;
  key[9]='\0';
  layer_info->mask.image->page.x+=layer_info->page.x;
  layer_info->mask.image->page.y+=layer_info->page.y;
  (void) SetImageRegistry(ImageRegistryType,(const char *) key,
    layer_info->mask.image,exception);
  (void) SetImageArtifact(layer_info->image,"psd:opacity-mask",
    (const char *) key);
  key_info=DestroyStringInfo(key_info);
  random_info=DestroyRandomInfo(random_info);
}
