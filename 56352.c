static Image *ReadINLINEImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  register size_t
    i;

  size_t
    quantum;

  ssize_t
    count;

  unsigned char
    *inline_image;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (LocaleNCompare(image_info->filename,"data:",5) == 0)
    {
      char
        *filename;

      Image
        *data_image;

      filename=AcquireString("data:");
      (void) ConcatenateMagickString(filename,image_info->filename,
        MaxTextExtent);
      data_image=ReadInlineImage(image_info,filename,exception);
      filename=DestroyString(filename);
      return(data_image);
    }
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  quantum=MagickMin((size_t) GetBlobSize(image),MagickMaxBufferExtent);
  if (quantum == 0)
    quantum=MagickMaxBufferExtent;
  inline_image=(unsigned char *) AcquireQuantumMemory(quantum,
    sizeof(*inline_image));
  count=0;
  for (i=0; inline_image != (unsigned char *) NULL; i+=count)
  {
    count=(ssize_t) ReadBlob(image,quantum,inline_image+i);
    if (count <= 0)
      {
        count=0;
        if (errno != EINTR)
          break;
      }
    if (~((size_t) i) < (quantum+1))
      {
        inline_image=(unsigned char *) RelinquishMagickMemory(inline_image);
        break;
      }
    inline_image=(unsigned char *) ResizeQuantumMemory(inline_image,i+count+
      quantum+1,sizeof(*inline_image));
  }
  if (inline_image == (unsigned char *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return((Image *) NULL);
    }
  inline_image[i+count]='\0';
  image=DestroyImageList(image);
  image=ReadInlineImage(image_info,(char *) inline_image,exception);
  inline_image=(unsigned char *) RelinquishMagickMemory(inline_image);
  return(image);
}
