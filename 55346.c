ModuleExport size_t RegisterWPGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("WPG");
  entry->decoder=(DecodeImageHandler *) ReadWPGImage;
  entry->magick=(IsImageFormatHandler *) IsWPG;
  entry->description=AcquireString("Word Perfect Graphics");
  entry->module=ConstantString("WPG");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
