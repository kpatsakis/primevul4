ModuleExport size_t RegisterINLINEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DATA");
  entry->decoder=(DecodeImageHandler *) ReadINLINEImage;
  entry->encoder=(EncodeImageHandler *) WriteINLINEImage;
  entry->format_type=ImplicitFormatType;
  entry->description=ConstantString("Base64-encoded inline images");
  entry->module=ConstantString("INLINE");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("INLINE");
  entry->decoder=(DecodeImageHandler *) ReadINLINEImage;
  entry->encoder=(EncodeImageHandler *) WriteINLINEImage;
  entry->format_type=ImplicitFormatType;
  entry->description=ConstantString("Base64-encoded inline images");
  entry->module=ConstantString("INLINE");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
