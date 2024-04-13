static MagickBooleanType ReadPSDChannelRaw(Image *image,const size_t channels,
  const ssize_t type,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  size_t
    count,
    row_size;

  ssize_t
    y;

  unsigned char
    *pixels;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "      layer data is RAW");

  row_size=GetPSDRowSize(image);
  pixels=(unsigned char *) AcquireQuantumMemory(row_size,sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);

  status=MagickTrue;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    status=MagickFalse;

    count=ReadBlob(image,row_size,pixels);
    if (count != row_size)
      break;

    status=ReadPSDChannelPixels(image,channels,y,type,pixels,exception);
    if (status == MagickFalse)
      break;
  }

  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  return(status);
}
