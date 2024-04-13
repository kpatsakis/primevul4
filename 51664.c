ModuleExport size_t RegisterDCMImage(void)
{
  MagickInfo
    *entry;

  static const char
    *DCMNote=
    {
      "DICOM is used by the medical community for images like X-rays.  The\n"
      "specification, \"Digital Imaging and Communications in Medicine\n"
      "(DICOM)\", is available at http://medical.nema.org/.  In particular,\n"
      "see part 5 which describes the image encoding (RLE, JPEG, JPEG-LS),\n"
      "and supplement 61 which adds JPEG-2000 encoding."
    };

  entry=AcquireMagickInfo("DCM","DCM",
    "Digital Imaging and Communications in Medicine image");
  entry->decoder=(DecodeImageHandler *) ReadDCMImage;
  entry->magick=(IsImageFormatHandler *) IsDCM;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderSeekableStreamFlag;
  entry->note=ConstantString(DCMNote);
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
