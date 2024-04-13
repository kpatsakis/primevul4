static xmlEntityPtr MSLGetEntity(void *context,const xmlChar *name)
{
  MSLInfo
    *msl_info;

  /*
    Get an entity by name.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.MSLGetEntity(%s)",(const char *) name);
  msl_info=(MSLInfo *) context;
  return(xmlGetDocEntity(msl_info->document,name));
}
