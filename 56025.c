static void JP2ErrorHandler(const char *message,void *client_data)
{
  ExceptionInfo
    *exception;

  exception=(ExceptionInfo *) client_data;
  (void) ThrowMagickException(exception,GetMagickModule(),CoderError,
    message,"`%s'","OpenJP2");
}
