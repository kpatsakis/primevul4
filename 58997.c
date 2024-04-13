ModuleExport size_t RegisterXCFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("XCF");
  entry->decoder=(DecodeImageHandler *) ReadXCFImage;
  entry->magick=(IsImageFormatHandler *) IsXCF;
  entry->description=ConstantString("GIMP image");
  entry->module=ConstantString("XCF");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
