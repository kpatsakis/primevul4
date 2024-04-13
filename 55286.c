ModuleExport void UnregisterYCBCRImage(void)
{
  (void) UnregisterMagickInfo("YCbCr");
  (void) UnregisterMagickInfo("YCbCrA");
}
