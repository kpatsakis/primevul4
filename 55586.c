ModuleExport size_t RegisterTXTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SPARSE-COLOR");
  entry->encoder=(EncodeImageHandler *) WriteTXTImage;
  entry->raw=MagickTrue;
  entry->endian_support=MagickTrue;
  entry->description=ConstantString("Sparse Color");
  entry->module=ConstantString("TXT");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("TEXT");
  entry->decoder=(DecodeImageHandler *) ReadTEXTImage;
  entry->encoder=(EncodeImageHandler *) WriteTXTImage;
  entry->raw=MagickTrue;
  entry->endian_support=MagickTrue;
  entry->format_type=ImplicitFormatType;
  entry->description=ConstantString("Text");
  entry->module=ConstantString("TXT");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("TXT");
  entry->decoder=(DecodeImageHandler *) ReadTXTImage;
  entry->encoder=(EncodeImageHandler *) WriteTXTImage;
  entry->description=ConstantString("Text");
  entry->magick=(IsImageFormatHandler *) IsTXT;
  entry->module=ConstantString("TXT");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
