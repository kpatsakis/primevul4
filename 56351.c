ModuleExport size_t RegisterPCXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DCX");
  entry->decoder=(DecodeImageHandler *) ReadPCXImage;
  entry->encoder=(EncodeImageHandler *) WritePCXImage;
  entry->seekable_stream=MagickTrue;
  entry->magick=(IsImageFormatHandler *) IsDCX;
  entry->description=ConstantString("ZSoft IBM PC multi-page Paintbrush");
  entry->module=ConstantString("PCX");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PCX");
  entry->decoder=(DecodeImageHandler *) ReadPCXImage;
  entry->encoder=(EncodeImageHandler *) WritePCXImage;
  entry->magick=(IsImageFormatHandler *) IsPCX;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("ZSoft IBM PC Paintbrush");
  entry->module=ConstantString("PCX");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
