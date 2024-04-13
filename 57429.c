MagickExport MagickBooleanType SyncImageSettings(const ImageInfo *image_info,
  Image *image)
{
  char
    property[MaxTextExtent];

  const char
    *option,
    *value;

  GeometryInfo
    geometry_info;

  MagickStatusType
    flags;

  ResolutionType
    units;

  /*
    Sync image options.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  option=GetImageOption(image_info,"background");
  if (option != (const char *) NULL)
    (void) QueryColorDatabase(option,&image->background_color,
      &image->exception);
  option=GetImageOption(image_info,"bias");
  if (option != (const char *) NULL)
    image->bias=StringToDoubleInterval(option,(double) QuantumRange+1.0);
  option=GetImageOption(image_info,"black-point-compensation");
  if (option != (const char *) NULL)
    image->black_point_compensation=(MagickBooleanType) ParseCommandOption(
      MagickBooleanOptions,MagickFalse,option);
  option=GetImageOption(image_info,"blue-primary");
  if (option != (const char *) NULL)
    {
      flags=ParseGeometry(option,&geometry_info);
      image->chromaticity.blue_primary.x=geometry_info.rho;
      image->chromaticity.blue_primary.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->chromaticity.blue_primary.y=image->chromaticity.blue_primary.x;
    }
  option=GetImageOption(image_info,"bordercolor");
  if (option != (const char *) NULL)
    (void) QueryColorDatabase(option,&image->border_color,&image->exception);
  option=GetImageOption(image_info,"colors");
  if (option != (const char *) NULL)
    image->colors=StringToUnsignedLong(option);
  option=GetImageOption(image_info,"compose");
  if (option != (const char *) NULL)
    image->compose=(CompositeOperator) ParseCommandOption(MagickComposeOptions,
      MagickFalse,option);
  option=GetImageOption(image_info,"compress");
  if (option != (const char *) NULL)
    image->compression=(CompressionType) ParseCommandOption(
      MagickCompressOptions,MagickFalse,option);
  option=GetImageOption(image_info,"debug");
  if (option != (const char *) NULL)
    image->debug=(MagickBooleanType) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,option);
  option=GetImageOption(image_info,"density");
  if (option != (const char *) NULL)
    {
      GeometryInfo
        geometry_info;

      /*
        Set image density.
      */
      flags=ParseGeometry(option,&geometry_info);
      image->x_resolution=geometry_info.rho;
      image->y_resolution=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->y_resolution=image->x_resolution;
    }
  option=GetImageOption(image_info,"depth");
  if (option != (const char *) NULL)
    image->depth=StringToUnsignedLong(option);
  option=GetImageOption(image_info,"endian");
  if (option != (const char *) NULL)
    image->endian=(EndianType) ParseCommandOption(MagickEndianOptions,
      MagickFalse,option);
  option=GetImageOption(image_info,"filter");
  if (option != (const char *) NULL)
    image->filter=(FilterTypes) ParseCommandOption(MagickFilterOptions,
      MagickFalse,option);
  option=GetImageOption(image_info,"fuzz");
  if (option != (const char *) NULL)
    image->fuzz=StringToDoubleInterval(option,(double) QuantumRange+1.0);
  option=GetImageOption(image_info,"gravity");
  if (option != (const char *) NULL)
    image->gravity=(GravityType) ParseCommandOption(MagickGravityOptions,
      MagickFalse,option);
  option=GetImageOption(image_info,"green-primary");
  if (option != (const char *) NULL)
    {
      flags=ParseGeometry(option,&geometry_info);
      image->chromaticity.green_primary.x=geometry_info.rho;
      image->chromaticity.green_primary.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->chromaticity.green_primary.y=image->chromaticity.green_primary.x;
    }
  option=GetImageOption(image_info,"intensity");
  if (option != (const char *) NULL)
    image->intensity=(PixelIntensityMethod) ParseCommandOption(
      MagickPixelIntensityOptions,MagickFalse,option);
  option=GetImageOption(image_info,"intent");
  if (option != (const char *) NULL)
    image->rendering_intent=(RenderingIntent) ParseCommandOption(
      MagickIntentOptions,MagickFalse,option);
  option=GetImageOption(image_info,"interlace");
  if (option != (const char *) NULL)
    image->interlace=(InterlaceType) ParseCommandOption(MagickInterlaceOptions,
      MagickFalse,option);
  option=GetImageOption(image_info,"interpolate");
  if (option != (const char *) NULL)
    image->interpolate=(InterpolatePixelMethod) ParseCommandOption(
      MagickInterpolateOptions,MagickFalse,option);
  option=GetImageOption(image_info,"loop");
  if (option != (const char *) NULL)
    image->iterations=StringToUnsignedLong(option);
  option=GetImageOption(image_info,"mattecolor");
  if (option != (const char *) NULL)
    (void) QueryColorDatabase(option,&image->matte_color,&image->exception);
  option=GetImageOption(image_info,"orient");
  if (option != (const char *) NULL)
    image->orientation=(OrientationType) ParseCommandOption(
      MagickOrientationOptions,MagickFalse,option);
  option=GetImageOption(image_info,"page");
  if (option != (const char *) NULL)
    {
      char
        *geometry;

      geometry=GetPageGeometry(option);
      flags=ParseAbsoluteGeometry(geometry,&image->page);
      geometry=DestroyString(geometry);
    }
  option=GetImageOption(image_info,"quality");
  if (option != (const char *) NULL)
    image->quality=StringToUnsignedLong(option);
  option=GetImageOption(image_info,"red-primary");
  if (option != (const char *) NULL)
    {
      flags=ParseGeometry(option,&geometry_info);
      image->chromaticity.red_primary.x=geometry_info.rho;
      image->chromaticity.red_primary.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->chromaticity.red_primary.y=image->chromaticity.red_primary.x;
    }
  if (image_info->quality != UndefinedCompressionQuality)
    image->quality=image_info->quality;
  option=GetImageOption(image_info,"scene");
  if (option != (const char *) NULL)
    image->scene=StringToUnsignedLong(option);
  option=GetImageOption(image_info,"taint");
  if (option != (const char *) NULL)
    image->taint=(MagickBooleanType) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,option);
  option=GetImageOption(image_info,"tile-offset");
  if (option != (const char *) NULL)
    {
      char
        *geometry;

      geometry=GetPageGeometry(option);
      flags=ParseAbsoluteGeometry(geometry,&image->tile_offset);
      geometry=DestroyString(geometry);
    }
  option=GetImageOption(image_info,"transparent-color");
  if (option != (const char *) NULL)
    (void) QueryColorDatabase(option,&image->transparent_color,
      &image->exception);
  option=GetImageOption(image_info,"type");
  if (option != (const char *) NULL)
    image->type=(ImageType) ParseCommandOption(MagickTypeOptions,MagickFalse,
      option);
  option=GetImageOption(image_info,"units");
  if (option != (const char *) NULL)
    units=(ResolutionType) ParseCommandOption(MagickResolutionOptions,
      MagickFalse,option);
  else
    units = image_info->units;
  if (units != UndefinedResolution)
    {
      if (image->units != units)
        switch (image->units)
        {
          case PixelsPerInchResolution:
          {
            if (units == PixelsPerCentimeterResolution)
              {
                image->x_resolution/=2.54;
                image->y_resolution/=2.54;
              }
            break;
          }
          case PixelsPerCentimeterResolution:
          {
            if (units == PixelsPerInchResolution)
              {
                image->x_resolution=(double) ((size_t) (100.0*2.54*
                  image->x_resolution+0.5))/100.0;
                image->y_resolution=(double) ((size_t) (100.0*2.54*
                  image->y_resolution+0.5))/100.0;
              }
            break;
          }
          default:
            break;
        }
      image->units=units;
    }
  option=GetImageOption(image_info,"white-point");
  if (option != (const char *) NULL)
    {
      flags=ParseGeometry(option,&geometry_info);
      image->chromaticity.white_point.x=geometry_info.rho;
      image->chromaticity.white_point.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->chromaticity.white_point.y=image->chromaticity.white_point.x;
    }
  ResetImageOptionIterator(image_info);
  for (option=GetNextImageOption(image_info); option != (const char *) NULL; )
  {
    value=GetImageOption(image_info,option);
    if (value != (const char *) NULL)
      {
        (void) FormatLocaleString(property,MaxTextExtent,"%s",option);
        (void) SetImageArtifact(image,property,value);
      }
    option=GetNextImageOption(image_info);
  }
  return(MagickTrue);
}
