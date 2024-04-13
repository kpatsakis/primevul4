static void ParseImageResourceBlocks(Image *image,
  const unsigned char *blocks,size_t length,
  MagickBooleanType *has_merged_image)
{
  const unsigned char
    *p;

  StringInfo
    *profile;

  unsigned int
    count,
    long_sans;

  unsigned short
    id,
    short_sans;

  if (length < 16)
    return;
  profile=BlobToStringInfo((const void *) NULL,length);
  SetStringInfoDatum(profile,blocks);
  (void) SetImageProfile(image,"8bim",profile);
  profile=DestroyStringInfo(profile);
  for (p=blocks; (p >= blocks) && (p < (blocks+length-16)); )
  {
    if (LocaleNCompare((const char *) p,"8BIM",4) != 0)
      break;
    p=PushLongPixel(MSBEndian,p,&long_sans);
    p=PushShortPixel(MSBEndian,p,&id);
    p=PushShortPixel(MSBEndian,p,&short_sans);
    p=PushLongPixel(MSBEndian,p,&count);
    if ((p+count) > (blocks+length-16))
      return;
    switch (id)
    {
      case 0x03ed:
      {
        char
          value[MaxTextExtent];

        unsigned short
          resolution;

        /*
          Resolution info.
        */
        p=PushShortPixel(MSBEndian,p,&resolution);
        image->x_resolution=(double) resolution;
        (void) FormatLocaleString(value,MaxTextExtent,"%g",
          image->x_resolution);
        (void) SetImageProperty(image,"tiff:XResolution",value);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&resolution);
        image->y_resolution=(double) resolution;
        (void) FormatLocaleString(value,MaxTextExtent,"%g",
          image->y_resolution);
        (void) SetImageProperty(image,"tiff:YResolution",value);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        image->units=PixelsPerInchResolution;
        break;
      }
      case 0x0421:
      {
        if (*(p+4) == 0)
          *has_merged_image=MagickFalse;
        p+=count;
        break;
      }
      default:
      {
        p+=count;
        break;
      }
    }
    if ((count & 0x01) != 0)
      p++;
  }
  return;
}
