static void LCMSExceptionHandler(cmsContext context,cmsUInt32Number severity,
  const char *message)
{
  Image
    *image;

  (void) LogMagickEvent(TransformEvent,GetMagickModule(),"lcms: #%u, %s",
    severity,message != (char *) NULL ? message : "no message");
  image=(Image *) context;
  if (image != (Image *) NULL)
    (void) ThrowMagickException(&image->exception,GetMagickModule(),
      ImageWarning,"UnableToTransformColorspace","`%s'",image->filename);
}
