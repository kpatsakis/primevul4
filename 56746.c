static MagickBooleanType WriteMPCImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent],
    cache_filename[MaxTextExtent];

  const char
    *property,
    *value;

  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    scene;

  register ssize_t
    i;

  size_t
    depth,
    one;

  /*
    Open persistent cache.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  (void) CopyMagickString(cache_filename,image->filename,MaxTextExtent);
  AppendImageFormat("cache",cache_filename);
  scene=0;
  offset=0;
  one=1;
  do
  {
    /*
      Write persistent cache meta-information.
    */
    depth=GetImageQuantumDepth(image,MagickTrue);
    if ((image->storage_class == PseudoClass) &&
        (image->colors > (one << depth)))
      image->storage_class=DirectClass;
    (void) WriteBlobString(image,"id=MagickCache\n");
    (void) FormatLocaleString(buffer,MaxTextExtent,"magick-signature=%u\n",
      GetMagickSignature((const StringInfo *) NULL));
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MaxTextExtent,
      "class=%s  colors=%.20g  matte=%s\n",CommandOptionToMnemonic(
      MagickClassOptions,image->storage_class),(double) image->colors,
      CommandOptionToMnemonic(MagickBooleanOptions,(ssize_t) image->matte));
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MaxTextExtent,
      "columns=%.20g  rows=%.20g depth=%.20g\n",(double) image->columns,
      (double) image->rows,(double) image->depth);
    (void) WriteBlobString(image,buffer);
    if (image->type != UndefinedType)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"type=%s\n",
          CommandOptionToMnemonic(MagickTypeOptions,image->type));
        (void) WriteBlobString(image,buffer);
      }
    if (image->colorspace != UndefinedColorspace)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"colorspace=%s\n",
          CommandOptionToMnemonic(MagickColorspaceOptions,image->colorspace));
        (void) WriteBlobString(image,buffer);
      }
    if (image->intensity != UndefinedPixelIntensityMethod)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"pixel-intensity=%s\n",
          CommandOptionToMnemonic(MagickPixelIntensityOptions,
          image->intensity));
        (void) WriteBlobString(image,buffer);
      }
    if (image->endian != UndefinedEndian)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"endian=%s\n",
          CommandOptionToMnemonic(MagickEndianOptions,image->endian));
        (void) WriteBlobString(image,buffer);
      }
    if (image->compression != UndefinedCompression)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "compression=%s  quality=%.20g\n",CommandOptionToMnemonic(
          MagickCompressOptions,image->compression),(double) image->quality);
        (void) WriteBlobString(image,buffer);
      }
    if (image->units != UndefinedResolution)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"units=%s\n",
          CommandOptionToMnemonic(MagickResolutionOptions,image->units));
        (void) WriteBlobString(image,buffer);
      }
    if ((image->x_resolution != 0) || (image->y_resolution != 0))
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "resolution=%gx%g\n",image->x_resolution,image->y_resolution);
        (void) WriteBlobString(image,buffer);
      }
    if ((image->page.width != 0) || (image->page.height != 0))
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "page=%.20gx%.20g%+.20g%+.20g\n",(double) image->page.width,(double)
          image->page.height,(double) image->page.x,(double) image->page.y);
        (void) WriteBlobString(image,buffer);
      }
    else
      if ((image->page.x != 0) || (image->page.y != 0))
        {
          (void) FormatLocaleString(buffer,MaxTextExtent,"page=%+ld%+ld\n",
            (long) image->page.x,(long) image->page.y);
          (void) WriteBlobString(image,buffer);
        }
    if ((image->tile_offset.x != 0) || (image->tile_offset.y != 0))
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"tile-offset=%+ld%+ld\n",
          (long) image->tile_offset.x,(long) image->tile_offset.y);
        (void) WriteBlobString(image,buffer);
      }
    if ((GetNextImageInList(image) != (Image *) NULL) ||
        (GetPreviousImageInList(image) != (Image *) NULL))
      {
        if (image->scene == 0)
          (void) FormatLocaleString(buffer,MaxTextExtent,
            "iterations=%.20g  delay=%.20g  ticks-per-second=%.20g\n",(double)
            image->iterations,(double) image->delay,(double)
            image->ticks_per_second);
        else
          (void) FormatLocaleString(buffer,MaxTextExtent,"scene=%.20g  "
            "iterations=%.20g  delay=%.20g  ticks-per-second=%.20g\n",
            (double) image->scene,(double) image->iterations,(double)
            image->delay,(double) image->ticks_per_second);
        (void) WriteBlobString(image,buffer);
      }
    else
      {
        if (image->scene != 0)
          {
            (void) FormatLocaleString(buffer,MaxTextExtent,"scene=%.20g\n",
              (double) image->scene);
            (void) WriteBlobString(image,buffer);
          }
        if (image->iterations != 0)
          {
            (void) FormatLocaleString(buffer,MaxTextExtent,"iterations=%.20g\n",
              (double) image->iterations);
            (void) WriteBlobString(image,buffer);
          }
        if (image->delay != 0)
          {
            (void) FormatLocaleString(buffer,MaxTextExtent,"delay=%.20g\n",
              (double) image->delay);
            (void) WriteBlobString(image,buffer);
          }
        if (image->ticks_per_second != UndefinedTicksPerSecond)
          {
            (void) FormatLocaleString(buffer,MaxTextExtent,
              "ticks-per-second=%.20g\n",(double) image->ticks_per_second);
            (void) WriteBlobString(image,buffer);
          }
      }
    if (image->gravity != UndefinedGravity)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"gravity=%s\n",
          CommandOptionToMnemonic(MagickGravityOptions,image->gravity));
        (void) WriteBlobString(image,buffer);
      }
    if (image->dispose != UndefinedDispose)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"dispose=%s\n",
          CommandOptionToMnemonic(MagickDisposeOptions,image->dispose));
        (void) WriteBlobString(image,buffer);
      }
    if (image->rendering_intent != UndefinedIntent)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "rendering-intent=%s\n",CommandOptionToMnemonic(MagickIntentOptions,
          image->rendering_intent));
        (void) WriteBlobString(image,buffer);
      }
    if (image->gamma != 0.0)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"gamma=%g\n",
          image->gamma);
        (void) WriteBlobString(image,buffer);
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        /*
          Note chomaticity points.
        */
        (void) FormatLocaleString(buffer,MaxTextExtent,"red-primary="
          "%g,%g  green-primary=%g,%g  blue-primary=%g,%g\n",
          image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
          image->chromaticity.green_primary.x,
          image->chromaticity.green_primary.y,
          image->chromaticity.blue_primary.x,
          image->chromaticity.blue_primary.y);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "white-point=%g,%g\n",image->chromaticity.white_point.x,
          image->chromaticity.white_point.y);
        (void) WriteBlobString(image,buffer);
      }
    if (image->orientation != UndefinedOrientation)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "orientation=%s\n",CommandOptionToMnemonic(MagickOrientationOptions,
          image->orientation));
        (void) WriteBlobString(image,buffer);
      }
    if (image->profiles != (void *) NULL)
      {
        const char
          *name;

        const StringInfo
          *profile;

        /*
          Generic profile.
        */
        ResetImageProfileIterator(image);
        for (name=GetNextImageProfile(image); name != (const char *) NULL; )
        {
          profile=GetImageProfile(image,name);
          if (profile != (StringInfo *) NULL)
            {
              (void) FormatLocaleString(buffer,MaxTextExtent,
                "profile:%s=%.20g\n",name,(double)
                GetStringInfoLength(profile));
              (void) WriteBlobString(image,buffer);
            }
          name=GetNextImageProfile(image);
        }
      }
    if (image->montage != (char *) NULL)
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"montage=%s\n",
          image->montage);
        (void) WriteBlobString(image,buffer);
      }
    ResetImagePropertyIterator(image);
    property=GetNextImageProperty(image);
    while (property != (const char *) NULL)
    {
      (void) FormatLocaleString(buffer,MaxTextExtent,"%s=",property);
      (void) WriteBlobString(image,buffer);
      value=GetImageProperty(image,property);
      if (value != (const char *) NULL)
        {
          size_t
            length;

          length=strlen(value);
          for (i=0; i < (ssize_t) length; i++)
            if (isspace((int) ((unsigned char) value[i])) != 0)
              break;
          if ((i == (ssize_t) length) && (i != 0))
            (void) WriteBlob(image,length,(const unsigned char *) value);
          else
            {
              (void) WriteBlobByte(image,'{');
              if (strchr(value,'}') == (char *) NULL)
                (void) WriteBlob(image,length,(const unsigned char *) value);
              else
                for (i=0; i < (ssize_t) length; i++)
                {
                  if (value[i] == (int) '}')
                    (void) WriteBlobByte(image,'\\');
                  (void) WriteBlobByte(image,value[i]);
                }
              (void) WriteBlobByte(image,'}');
            }
        }
      (void) WriteBlobByte(image,'\n');
      property=GetNextImageProperty(image);
    }
