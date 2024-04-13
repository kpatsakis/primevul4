ModuleExport size_t RegisterMSLImage(void)
{
  MagickInfo
    *entry;

#if defined(MAGICKCORE_XML_DELEGATE)
  xmlInitParser();
#endif
  entry=AcquireMagickInfo("MSL","MSL","Magick Scripting Language");
#if defined(MAGICKCORE_XML_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadMSLImage;
  entry->encoder=(EncodeImageHandler *) WriteMSLImage;
#endif
  entry->format_type=ImplicitFormatType;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
