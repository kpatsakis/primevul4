ModuleExport void UnregisterDDSImage(void)
{
  (void) UnregisterMagickInfo("DDS");
  (void) UnregisterMagickInfo("DXT1");
  (void) UnregisterMagickInfo("DXT5");
}
