static void MagickPNGErrorHandler(png_struct *ping,png_const_charp message)
{
  ExceptionInfo
    *exception;

  Image
    *image;

  PNGErrorInfo
    *error_info;

  error_info=(PNGErrorInfo *) png_get_error_ptr(ping);
  image=error_info->image;
  exception=error_info->exception;

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  libpng-%s error: %s", png_get_libpng_ver(NULL),message);

  (void) ThrowMagickException(exception,GetMagickModule(),CoderError,message,
    "`%s'",image->filename);

#if (PNG_LIBPNG_VER < 10500)
  /* A warning about deprecated use of jmpbuf here is unavoidable if you
   * are building with libpng-1.4.x and can be ignored.
   */
  longjmp(ping->jmpbuf,1);
#else
  png_longjmp(ping,1);
#endif
}
