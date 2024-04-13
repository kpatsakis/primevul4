MagickExport MagickBooleanType FormatImageProperty(Image *image,
  const char *property,const char *format,...)
{
  char
    value[MagickPathExtent];

  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  ssize_t
    n;

  va_list
    operands;

  va_start(operands,format);
  n=FormatLocaleStringList(value,MagickPathExtent,format,operands);
  (void) n;
  va_end(operands);
  exception=AcquireExceptionInfo();
  status=SetImageProperty(image,property,value,exception);
  exception=DestroyExceptionInfo(exception);
  return(status);
}
