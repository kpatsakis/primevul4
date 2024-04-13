static MagickBooleanType IsPostscriptRendered(const char *path)
{
  MagickBooleanType
    status;

  struct stat
    attributes;

  if ((path == (const char *) NULL) || (*path == '\0'))
    return(MagickFalse);
  status=GetPathAttributes(path,&attributes);
  if ((status != MagickFalse) && S_ISREG(attributes.st_mode) &&
      (attributes.st_size > 0))
    return(MagickTrue);
  return(MagickFalse);
}
