static int MSLIsStandalone(void *context)
{
  MSLInfo
    *msl_info;

  /*
    Is this document tagged standalone?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.MSLIsStandalone()");
  msl_info=(MSLInfo *) context;
  return(msl_info->document->standalone == 1);
}
