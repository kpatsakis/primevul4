static void MSLInternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  MSLInfo
    *msl_info;

  /*
    Does this document has an internal subset?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.internalSubset(%s %s %s)",name,
    (external_id != (const xmlChar *) NULL ? (const char *) external_id : " "),
    (system_id != (const xmlChar *) NULL ? (const char *) system_id : " "));
  msl_info=(MSLInfo *) context;
  (void) xmlCreateIntSubset(msl_info->document,name,external_id,system_id);
}
