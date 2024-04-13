ModuleExport size_t RegisterPALMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PALM");
  entry->decoder=(DecodeImageHandler *) ReadPALMImage;
  entry->encoder=(EncodeImageHandler *) WritePALMImage;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Palm pixmap");
  entry->module=ConstantString("PALM");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
