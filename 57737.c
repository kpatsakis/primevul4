ModuleExport size_t RegisterARTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("ART");
  entry->decoder=(DecodeImageHandler *) ReadARTImage;
  entry->encoder=(EncodeImageHandler *) WriteARTImage;
  entry->raw=MagickTrue;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("PFS: 1st Publisher Clip Art");
  entry->module=ConstantString("ART");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
