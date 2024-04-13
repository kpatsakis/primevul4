ModuleExport void UnregisterCALSImage(void)
{
  (void) UnregisterMagickInfo("CAL");
  (void) UnregisterMagickInfo("CALS");
}
