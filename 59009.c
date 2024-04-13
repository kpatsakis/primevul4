ModuleExport size_t RegisterTGAImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("ICB");
  entry->decoder=(DecodeImageHandler *) ReadTGAImage;
  entry->encoder=(EncodeImageHandler *) WriteTGAImage;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("Truevision Targa image");
  entry->module=ConstantString("TGA");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("TGA");
  entry->decoder=(DecodeImageHandler *) ReadTGAImage;
  entry->encoder=(EncodeImageHandler *) WriteTGAImage;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("Truevision Targa image");
  entry->module=ConstantString("TGA");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("VDA");
  entry->decoder=(DecodeImageHandler *) ReadTGAImage;
  entry->encoder=(EncodeImageHandler *) WriteTGAImage;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("Truevision Targa image");
  entry->module=ConstantString("TGA");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("VST");
  entry->decoder=(DecodeImageHandler *) ReadTGAImage;
  entry->encoder=(EncodeImageHandler *) WriteTGAImage;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("Truevision Targa image");
  entry->module=ConstantString("TGA");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
