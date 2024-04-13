static MagickBooleanType WriteDDSImage(const ImageInfo *image_info,
  Image *image)
{
  const char
    *option;

  size_t
    compression,
    columns,
    maxMipmaps,
    mipmaps,
    pixelFormat,
    rows;

  MagickBooleanType
    clusterFit,
    status,
    weightByAlpha;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  (void) TransformImageColorspace(image,sRGBColorspace);
  pixelFormat=DDPF_FOURCC;
  compression=FOURCC_DXT5;

  if (!image->matte)
    compression=FOURCC_DXT1;

  if (LocaleCompare(image_info->magick,"dxt1") == 0)
    compression=FOURCC_DXT1;

  option=GetImageOption(image_info,"dds:compression");
  if (option != (char *) NULL)
    {
       if (LocaleCompare(option,"dxt1") == 0)
         compression=FOURCC_DXT1;
       if (LocaleCompare(option,"none") == 0)
         pixelFormat=DDPF_RGB;
    }

  clusterFit=MagickFalse;
  weightByAlpha=MagickFalse;

  if (pixelFormat == DDPF_FOURCC)
    {
      option=GetImageOption(image_info,"dds:cluster-fit");
      if (IsStringTrue(option) != MagickFalse)
        {
          clusterFit=MagickTrue;
          if (compression != FOURCC_DXT1)
            {
              option=GetImageOption(image_info,"dds:weight-by-alpha");
              if (IsStringTrue(option) != MagickFalse)
                weightByAlpha=MagickTrue;
            }
        }
    }

  maxMipmaps=SIZE_MAX;
  mipmaps=0;
  if ((image->columns & (image->columns - 1)) == 0 &&
      (image->rows & (image->rows - 1)) == 0)
    {
      option=GetImageOption(image_info,"dds:mipmaps");
      if (option != (char *) NULL)
        maxMipmaps=StringToUnsignedLong(option);

      if (maxMipmaps != 0)
        {
          columns=image->columns;
          rows=image->rows;
          while ((columns != 1 || rows != 1) && mipmaps != maxMipmaps)
          {
            columns=DIV2(columns);
            rows=DIV2(rows);
            mipmaps++;
          }
        }
    }

  WriteDDSInfo(image,pixelFormat,compression,mipmaps);

  WriteImageData(image,pixelFormat,compression,clusterFit,weightByAlpha,
    &image->exception);

  if (mipmaps > 0 && WriteMipmaps(image,pixelFormat,compression,mipmaps,
        clusterFit,weightByAlpha,&image->exception) == MagickFalse)
    return(MagickFalse);

  (void) CloseBlob(image);
  return(MagickTrue);
}
