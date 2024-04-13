ModuleExport size_t RegisterMAPImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MAP");
  entry->decoder=(DecodeImageHandler *) ReadMAPImage;
  entry->encoder=(EncodeImageHandler *) WriteMAPImage;
  entry->adjoin=MagickFalse;
  entry->format_type=ExplicitFormatType;
  entry->raw=MagickTrue;
  entry->endian_support=MagickTrue;
  entry->description=ConstantString("Colormap intensities and indices");
  entry->module=ConstantString("MAP");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
