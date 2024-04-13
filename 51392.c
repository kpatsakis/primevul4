static MagickBooleanType SkipXMPValue(const char *value)
{
  if (value == (const char*) NULL)
    return(MagickTrue);
  while (*value != '\0')
  {
    if (isspace((int) ((unsigned char) *value)) == 0)
      return(MagickFalse);
    value++;
  }
  return(MagickTrue);
}
