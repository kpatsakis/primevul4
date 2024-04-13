static int MSLHasInternalSubset(void *context)
{
  MSLInfo
    *msl_info;

  /*
    Does this document has an internal subset?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.MSLHasInternalSubset()");
  msl_info=(MSLInfo *) context;
  return(msl_info->document->intSubset != NULL);
}
