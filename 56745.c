ModuleExport void UnregisterMPCImage(void)
{
  (void) UnregisterMagickInfo("CACHE");
  (void) UnregisterMagickInfo("MPC");
}
