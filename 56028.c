static void JP2WarningHandler(const char *message,void *client_data)
{
  ExceptionInfo
    *exception;

  exception=(ExceptionInfo *) client_data;
  (void) ThrowMagickException(exception,GetMagickModule(),CoderWarning,
    message,"`%s'","OpenJP2");
}
