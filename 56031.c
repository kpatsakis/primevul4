ModuleExport void UnregisterJP2Image(void)
{
  (void) UnregisterMagickInfo("JPC");
  (void) UnregisterMagickInfo("JPT");
  (void) UnregisterMagickInfo("JPM");
  (void) UnregisterMagickInfo("JP2");
  (void) UnregisterMagickInfo("J2K");
}
