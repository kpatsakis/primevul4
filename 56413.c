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

  entry=SetMagickInfo("DCM");
  entry->decoder=(DecodeImageHandler *) ReadDCMImage;
  entry->magick=(IsImageFormatHandler *) IsDCM;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString(
    "Digital Imaging and Communications in Medicine image");
  entry->note=ConstantString(DCMNote);
  entry->module=ConstantString("DCM");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
