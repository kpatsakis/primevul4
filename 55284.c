static MagickBooleanType WriteYUVImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  Image
    *chroma_image,
    *yuv_image;

  InterlaceType
    interlace;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  register const Quantum
    *p,
    *s;

  register ssize_t
    x;

  size_t
    height,
    quantum,
    width;

  ssize_t
    horizontal_factor,
    vertical_factor,
    y;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  quantum=(size_t) (image->depth <= 8 ? 1 : 2);
  interlace=image->interlace;
  horizontal_factor=2;
  vertical_factor=2;
  if (image_info->sampling_factor != (char *) NULL)
    {
      GeometryInfo
        geometry_info;

      MagickStatusType
        flags;

      flags=ParseGeometry(image_info->sampling_factor,&geometry_info);
      horizontal_factor=(ssize_t) geometry_info.rho;
      vertical_factor=(ssize_t) geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        vertical_factor=horizontal_factor;
      if ((horizontal_factor != 1) && (horizontal_factor != 2) &&
          (vertical_factor != 1) && (vertical_factor != 2))
        ThrowWriterException(CorruptImageError,"UnexpectedSamplingFactor");
    }
  if ((interlace == UndefinedInterlace) ||
      ((interlace == NoInterlace) && (vertical_factor == 2)))
    {
      interlace=NoInterlace;    /* CCIR 4:2:2 */
      if (vertical_factor == 2)
        interlace=PlaneInterlace; /* CCIR 4:1:1 */
    }
  if (interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
      if (status == MagickFalse)
        return(status);
    }
  else
    {
      AppendImageFormat("Y",image->filename);
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
      if (status == MagickFalse)
        return(status);
    }
  scene=0;
  do
  {
    /*
      Sample image to an even width and height, if necessary.
    */
    image->depth=(size_t) (quantum == 1 ? 8 : 16);
    width=image->columns+(image->columns & (horizontal_factor-1));
    height=image->rows+(image->rows & (vertical_factor-1));
    yuv_image=ResizeImage(image,width,height,TriangleFilter,exception);
    if (yuv_image == (Image *) NULL)
      {
        (void) CloseBlob(image);
        return(MagickFalse);
      }
    (void) TransformImageColorspace(yuv_image,YCbCrColorspace,exception);
    /*
      Downsample image.
    */
    chroma_image=ResizeImage(image,width/horizontal_factor,
      height/vertical_factor,TriangleFilter,exception);
    if (chroma_image == (Image *) NULL)
      {
        (void) CloseBlob(image);
        return(MagickFalse);
      }
    (void) TransformImageColorspace(chroma_image,YCbCrColorspace,exception);
    if (interlace == NoInterlace)
      {
        /*
          Write noninterlaced YUV.
        */
        for (y=0; y < (ssize_t) yuv_image->rows; y++)
        {
          p=GetVirtualPixels(yuv_image,0,y,yuv_image->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          s=GetVirtualPixels(chroma_image,0,y,chroma_image->columns,1,
            exception);
          if (s == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) yuv_image->columns; x+=2)
          {
            if (quantum == 1)
              {
                (void) WriteBlobByte(image,ScaleQuantumToChar(
                  GetPixelGreen(yuv_image,s)));
                (void) WriteBlobByte(image,ScaleQuantumToChar(
                  GetPixelRed(yuv_image,p)));
                p+=GetPixelChannels(yuv_image);
                (void) WriteBlobByte(image,ScaleQuantumToChar(
                  GetPixelBlue(yuv_image,s)));
                (void) WriteBlobByte(image,ScaleQuantumToChar(
                  GetPixelRed(yuv_image,p)));
              }
            else
              {
                (void) WriteBlobByte(image,ScaleQuantumToChar(
                  GetPixelGreen(yuv_image,s)));
                (void) WriteBlobShort(image,ScaleQuantumToShort(
                  GetPixelRed(yuv_image,p)));
                p+=GetPixelChannels(yuv_image);
                (void) WriteBlobByte(image,ScaleQuantumToChar(
                  GetPixelBlue(yuv_image,s)));
                (void) WriteBlobShort(image,ScaleQuantumToShort(
                  GetPixelRed(yuv_image,p)));
              }
            p+=GetPixelChannels(yuv_image);
            s++;
          }
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        yuv_image=DestroyImage(yuv_image);
      }
    else
      {
        /*
          Initialize Y channel.
        */
        for (y=0; y < (ssize_t) yuv_image->rows; y++)
        {
          p=GetVirtualPixels(yuv_image,0,y,yuv_image->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) yuv_image->columns; x++)
          {
            if (quantum == 1)
              (void) WriteBlobByte(image,ScaleQuantumToChar(
                GetPixelRed(yuv_image,p)));
            else
              (void) WriteBlobShort(image,ScaleQuantumToShort(
                GetPixelRed(yuv_image,p)));
            p+=GetPixelChannels(yuv_image);
          }
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        yuv_image=DestroyImage(yuv_image);
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,SaveImageTag,1,3);
            if (status == MagickFalse)
              break;
          }
        /*
          Initialize U channel.
        */
        if (interlace == PartitionInterlace)
          {
            (void) CloseBlob(image);
            AppendImageFormat("U",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
            if (status == MagickFalse)
              return(status);
          }
        for (y=0; y < (ssize_t) chroma_image->rows; y++)
        {
          p=GetVirtualPixels(chroma_image,0,y,chroma_image->columns,1,
            exception);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) chroma_image->columns; x++)
          {
            if (quantum == 1)
              (void) WriteBlobByte(image,ScaleQuantumToChar(
                GetPixelGreen(chroma_image,p)));
            else
              (void) WriteBlobShort(image,ScaleQuantumToShort(
                GetPixelGreen(chroma_image,p)));
            p+=GetPixelChannels(chroma_image);
          }
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,SaveImageTag,2,3);
            if (status == MagickFalse)
              break;
          }
        /*
          Initialize V channel.
        */
        if (interlace == PartitionInterlace)
          {
            (void) CloseBlob(image);
            AppendImageFormat("V",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
            if (status == MagickFalse)
              return(status);
          }
        for (y=0; y < (ssize_t) chroma_image->rows; y++)
        {
          p=GetVirtualPixels(chroma_image,0,y,chroma_image->columns,1,
            exception);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) chroma_image->columns; x++)
          {
            if (quantum == 1)
              (void) WriteBlobByte(image,ScaleQuantumToChar(
                GetPixelBlue(chroma_image,p)));
            else
              (void) WriteBlobShort(image,ScaleQuantumToShort(
                GetPixelBlue(chroma_image,p)));
            p+=GetPixelChannels(chroma_image);
          }
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,SaveImageTag,2,3);
            if (status == MagickFalse)
              break;
          }
      }
    chroma_image=DestroyImage(chroma_image);
    if (interlace == PartitionInterlace)
      (void) CopyMagickString(image->filename,image_info->filename,
        MagickPathExtent);
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,
      GetImageListLength(image));
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(MagickTrue);
}
