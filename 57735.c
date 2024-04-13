ModuleExport size_t RegisterRLEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("RLE");
  entry->decoder=(DecodeImageHandler *) ReadRLEImage;
  entry->magick=(IsImageFormatHandler *) IsRLE;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("Utah Run length encoded image");
  entry->module=ConstantString("RLE");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
