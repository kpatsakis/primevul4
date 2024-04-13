ModuleExport void UnregisterPSImage(void)
{
  (void) UnregisterMagickInfo("EPI");
  (void) UnregisterMagickInfo("EPS");
  (void) UnregisterMagickInfo("EPSF");
  (void) UnregisterMagickInfo("EPSI");
  (void) UnregisterMagickInfo("PS");
}
