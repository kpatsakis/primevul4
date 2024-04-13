static void MSLSetDocumentLocator(void *context,xmlSAXLocatorPtr location)
{
  MSLInfo
    *msl_info;

  /*
    Receive the document locator at startup, actually xmlDefaultSAXLocator.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.setDocumentLocator()\n");
  (void) location;
  msl_info=(MSLInfo *) context;
  (void) msl_info;
}
