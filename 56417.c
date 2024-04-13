static void MSLComment(void *context,const xmlChar *value)
{
  MSLInfo
    *msl_info;

  /*
    A comment has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.comment(%s)",value);
  msl_info=(MSLInfo *) context;
  (void) msl_info;
}
