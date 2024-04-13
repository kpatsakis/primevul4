static void TIFFErrors(const char *module,const char *format,va_list error)
{
  char
    message[MaxTextExtent];

  ExceptionInfo
    *exception;

#if defined(MAGICKCORE_HAVE_VSNPRINTF)
  (void) vsnprintf(message,MaxTextExtent,format,error);
#else
  (void) vsprintf(message,format,error);
#endif
  (void) ConcatenateMagickString(message,".",MaxTextExtent);
  exception=(ExceptionInfo *) GetMagickThreadValue(tiff_exception);
  if (exception != (ExceptionInfo *) NULL)
    (void) ThrowMagickException(exception,GetMagickModule(),CoderError,message,
      "`%s'",module);
}
