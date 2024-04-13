static Image *ReadMSLImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=(Image *) NULL;
  (void) ProcessMSLScript(image_info,&image,exception);
  return(GetFirstImageInList(image));
}
