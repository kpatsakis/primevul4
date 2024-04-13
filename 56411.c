ModuleExport void UnregisterPDFImage(void)
{
  (void) UnregisterMagickInfo("AI");
  (void) UnregisterMagickInfo("EPDF");
  (void) UnregisterMagickInfo("PDF");
  (void) UnregisterMagickInfo("PDFA");
}
