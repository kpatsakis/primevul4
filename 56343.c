ModuleExport size_t RegisterCALSImage(void)
{
  MagickInfo
    *entry;

  static const char
    *CALSDescription=
    {
      "Continuous Acquisition and Life-cycle Support Type 1"
    },
    *CALSNote=
    {
      "Specified in MIL-R-28002 and MIL-PRF-28002"
    };

  entry=SetMagickInfo("CAL");
  entry->decoder=(DecodeImageHandler *) ReadCALSImage;
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->encoder=(EncodeImageHandler *) WriteCALSImage;
#endif
  entry->adjoin=MagickFalse;
  entry->magick=(IsImageFormatHandler *) IsCALS;
  entry->description=ConstantString(CALSDescription);
  entry->note=ConstantString(CALSNote);
  entry->module=ConstantString("CALS");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("CALS");
  entry->decoder=(DecodeImageHandler *) ReadCALSImage;
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->encoder=(EncodeImageHandler *) WriteCALSImage;
#endif
  entry->adjoin=MagickFalse;
  entry->magick=(IsImageFormatHandler *) IsCALS;
  entry->description=ConstantString(CALSDescription);
  entry->note=ConstantString(CALSNote);
  entry->module=ConstantString("CALS");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
