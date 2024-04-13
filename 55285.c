ModuleExport size_t RegisterYCBCRImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("YCbCr","YCbCr","Raw Y, Cb, and Cr samples");
  entry->decoder=(DecodeImageHandler *) ReadYCBCRImage;
  entry->encoder=(EncodeImageHandler *) WriteYCBCRImage;
  entry->flags|=CoderRawSupportFlag;
  entry->flags|=CoderEndianSupportFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("YCbCr","YCbCrA","Raw Y, Cb, Cr, and alpha samples");
  entry->decoder=(DecodeImageHandler *) ReadYCBCRImage;
  entry->encoder=(EncodeImageHandler *) WriteYCBCRImage;
  entry->flags|=CoderRawSupportFlag;
  entry->flags|=CoderEndianSupportFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
