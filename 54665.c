ModuleExport size_t RegisterWPGImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("WPG","WPG","Word Perfect Graphics");
  entry->decoder=(DecodeImageHandler *) ReadWPGImage;
  entry->magick=(IsImageFormatHandler *) IsWPG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
