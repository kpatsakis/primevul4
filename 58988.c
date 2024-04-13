ModuleExport size_t RegisterWEBPImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
  entry=SetMagickInfo("WEBP");
#if defined(MAGICKCORE_WEBP_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadWEBPImage;
  entry->encoder=(EncodeImageHandler *) WriteWEBPImage;
  (void) FormatLocaleString(version,MaxTextExtent,"libwebp %d.%d.%d[%04X]",
    (WebPGetDecoderVersion() >> 16) & 0xff,
    (WebPGetDecoderVersion() >> 8) & 0xff,
    (WebPGetDecoderVersion() >> 0) & 0xff,WEBP_DECODER_ABI_VERSION);
#endif
  entry->description=ConstantString("WebP Image Format");
  entry->mime_type=ConstantString("image/webp");
  entry->adjoin=MagickFalse;
  entry->module=ConstantString("WEBP");
  entry->magick=(IsImageFormatHandler *) IsWEBP;
  if (*version != '\0')
    entry->version=ConstantString(version);
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
