static MagickBooleanType WriteWEBPImage(const ImageInfo *image_info,
  Image *image)
{
  const char
    *value;

  int
    webp_status;

  MagickBooleanType
    status;

  MemoryInfo
    *pixel_info;

  register uint32_t
    *magick_restrict q;

  ssize_t
    y;

  WebPConfig
    configure;

  WebPPicture
    picture;

  WebPAuxStats
    statistics;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->columns > 16383UL) || (image->rows > 16383UL))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  if ((WebPPictureInit(&picture) == 0) || (WebPConfigInit(&configure) == 0))
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");
  picture.writer=WebPEncodeWriter;
  picture.custom_ptr=(void *) image;
#if WEBP_DECODER_ABI_VERSION >= 0x0100
  picture.progress_hook=WebPEncodeProgress;
#endif
  picture.stats=(&statistics);
  picture.width=(int) image->columns;
  picture.height=(int) image->rows;
  picture.argb_stride=(int) image->columns;
  picture.use_argb=1;
  if (image->quality != UndefinedCompressionQuality)
    configure.quality=(float) image->quality;
  if (image->quality >= 100)
    configure.lossless=1;
  value=GetImageOption(image_info,"webp:lossless");
  if (value != (char *) NULL)
    configure.lossless=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:method");
  if (value != (char *) NULL)
    configure.method=StringToInteger(value);
  value=GetImageOption(image_info,"webp:image-hint");
  if (value != (char *) NULL)
    {
      if (LocaleCompare(value,"default") == 0)
        configure.image_hint=WEBP_HINT_DEFAULT;
      if (LocaleCompare(value,"photo") == 0)
        configure.image_hint=WEBP_HINT_PHOTO;
      if (LocaleCompare(value,"picture") == 0)
        configure.image_hint=WEBP_HINT_PICTURE;
#if WEBP_DECODER_ABI_VERSION >= 0x0200
      if (LocaleCompare(value,"graph") == 0)
        configure.image_hint=WEBP_HINT_GRAPH;
#endif
    }
  value=GetImageOption(image_info,"webp:target-size");
  if (value != (char *) NULL)
    configure.target_size=StringToInteger(value);
  value=GetImageOption(image_info,"webp:target-psnr");
  if (value != (char *) NULL)
    configure.target_PSNR=(float) StringToDouble(value,(char **) NULL);
  value=GetImageOption(image_info,"webp:segments");
  if (value != (char *) NULL)
    configure.segments=StringToInteger(value);
  value=GetImageOption(image_info,"webp:sns-strength");
  if (value != (char *) NULL)
    configure.sns_strength=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-strength");
  if (value != (char *) NULL)
    configure.filter_strength=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-sharpness");
  if (value != (char *) NULL)
    configure.filter_sharpness=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-type");
  if (value != (char *) NULL)
    configure.filter_type=StringToInteger(value);
  value=GetImageOption(image_info,"webp:auto-filter");
  if (value != (char *) NULL)
    configure.autofilter=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:alpha-compression");
  if (value != (char *) NULL)
    configure.alpha_compression=StringToInteger(value);
  value=GetImageOption(image_info,"webp:alpha-filtering");
  if (value != (char *) NULL)
    configure.alpha_filtering=StringToInteger(value);
  value=GetImageOption(image_info,"webp:alpha-quality");
  if (value != (char *) NULL)
    configure.alpha_quality=StringToInteger(value);
  value=GetImageOption(image_info,"webp:pass");
  if (value != (char *) NULL)
    configure.pass=StringToInteger(value);
  value=GetImageOption(image_info,"webp:show-compressed");
  if (value != (char *) NULL)
    configure.show_compressed=StringToInteger(value);
  value=GetImageOption(image_info,"webp:preprocessing");
  if (value != (char *) NULL)
    configure.preprocessing=StringToInteger(value);
  value=GetImageOption(image_info,"webp:partitions");
  if (value != (char *) NULL)
    configure.partitions=StringToInteger(value);
  value=GetImageOption(image_info,"webp:partition-limit");
  if (value != (char *) NULL)
    configure.partition_limit=StringToInteger(value);
#if WEBP_DECODER_ABI_VERSION >= 0x0201
  value=GetImageOption(image_info,"webp:emulate-jpeg-size");
  if (value != (char *) NULL)
    configure.emulate_jpeg_size=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:low-memory");
  if (value != (char *) NULL)
    configure.low_memory=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:thread-level");
  if (value != (char *) NULL)
    configure.thread_level=StringToInteger(value);
#endif
  if (WebPValidateConfig(&configure) == 0)
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");
  /*
    Allocate memory for pixels.
  */
  (void) TransformImageColorspace(image,sRGBColorspace);
  pixel_info=AcquireVirtualMemory(image->columns,image->rows*
    sizeof(*picture.argb));
  if (pixel_info == (MemoryInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  picture.argb=(uint32_t *) GetVirtualMemoryBlob(pixel_info);
  /*
    Convert image to WebP raster pixels.
  */
  q=picture.argb;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const PixelPacket
      *magick_restrict p;

    register ssize_t
      x;

    p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
    if (p == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      *q++=(uint32_t) (image->matte != MagickFalse ?
        ScaleQuantumToChar(GetPixelAlpha(p)) << 24 : 0xff000000u) |
        (ScaleQuantumToChar(GetPixelRed(p)) << 16) |
        (ScaleQuantumToChar(GetPixelGreen(p)) << 8) |
        (ScaleQuantumToChar(GetPixelBlue(p)));
      p++;
    }
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  webp_status=WebPEncode(&configure,&picture);
  if (webp_status == 0)
    {
      const char
        *message;

      switch (picture.error_code)
      {
        case VP8_ENC_ERROR_OUT_OF_MEMORY:
        {
          message="out of memory";
          break;
        }
        case VP8_ENC_ERROR_BITSTREAM_OUT_OF_MEMORY:
        {
          message="bitstream out of memory";
          break;
        }
        case VP8_ENC_ERROR_NULL_PARAMETER:
        {
          message="NULL parameter";
          break;
        }
        case VP8_ENC_ERROR_INVALID_CONFIGURATION:
        {
          message="invalid configuration";
          break;
        }
        case VP8_ENC_ERROR_BAD_DIMENSION:
        {
          message="bad dimension";
          break;
        }
        case VP8_ENC_ERROR_PARTITION0_OVERFLOW:
        {
          message="partition 0 overflow (> 512K)";
          break;
        }
        case VP8_ENC_ERROR_PARTITION_OVERFLOW:
        {
          message="partition overflow (> 16M)";
          break;
        }
        case VP8_ENC_ERROR_BAD_WRITE:
        {
          message="bad write";
          break;
        }
        case VP8_ENC_ERROR_FILE_TOO_BIG:
        {
          message="file too big (> 4GB)";
          break;
        }
#if WEBP_DECODER_ABI_VERSION >= 0x0100
        case VP8_ENC_ERROR_USER_ABORT:
        {
          message="user abort";
          break;
        }
#endif
        default:
        {
          message="unknown exception";
          break;
        }
      }
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        CorruptImageError,(char *) message,"`%s'",image->filename);
    }
  picture.argb=(uint32_t *) NULL;
  WebPPictureFree(&picture);
  pixel_info=RelinquishVirtualMemory(pixel_info);
  (void) CloseBlob(image);
  return(webp_status == 0 ? MagickFalse : MagickTrue);
}
