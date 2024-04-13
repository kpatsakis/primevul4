ModuleExport size_t RegisterPICTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCT");
  entry->decoder=(DecodeImageHandler *) ReadPICTImage;
  entry->encoder=(EncodeImageHandler *) WritePICTImage;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Apple Macintosh QuickDraw/PICT");
  entry->magick=(IsImageFormatHandler *) IsPICT;
  entry->module=ConstantString("PICT");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PICT");
  entry->decoder=(DecodeImageHandler *) ReadPICTImage;
  entry->encoder=(EncodeImageHandler *) WritePICTImage;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Apple Macintosh QuickDraw/PICT");
  entry->magick=(IsImageFormatHandler *) IsPICT;
  entry->module=ConstantString("PICT");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
