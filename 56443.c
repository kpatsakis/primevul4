ModuleExport void UnregisterMSLImage(void)
{
  (void) UnregisterMagickInfo("MSL");
#if defined(MAGICKCORE_XML_DELEGATE)
  xmlCleanupParser();
#endif
}
