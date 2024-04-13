static void GetProfilesFromResourceBlock(Image *image,
  const StringInfo *resource_block)
{
  const unsigned char
    *datum;

  register const unsigned char
    *p;

  size_t
    length;

  ssize_t
    count;

  StringInfo
    *profile;

  unsigned char
    length_byte;

  unsigned int
    value;

  unsigned short
    id;

  datum=GetStringInfoDatum(resource_block);
  length=GetStringInfoLength(resource_block);
  for (p=datum; p < (datum+length-16); )
  {
    if (LocaleNCompare((char *) p,"8BIM",4) != 0)
      break;
    p+=4;
    p=ReadResourceShort(p,&id);
    p=ReadResourceByte(p,&length_byte);
    p+=length_byte;
    if (((length_byte+1) & 0x01) != 0)
      p++;
    if (p > (datum+length-4))
      break;
    p=ReadResourceLong(p,&value);
    count=(ssize_t) value;
    if ((p > (datum+length-count)) || (count > (ssize_t) length) ||
        (count < 0))
      break;
    switch (id)
    {
      case 0x03ed:
      {
        unsigned int
          resolution;

        unsigned short
          units;

        /*
          Resolution.
        */
        p=ReadResourceLong(p,&resolution);
        image->x_resolution=((double) resolution)/65536.0;
        p=ReadResourceShort(p,&units)+2;
        p=ReadResourceLong(p,&resolution)+4;
        image->y_resolution=((double) resolution)/65536.0;
        /*
          Values are always stored as pixels per inch.
        */
        if ((ResolutionType) units != PixelsPerCentimeterResolution)
          image->units=PixelsPerInchResolution;
        else
          {
            image->units=PixelsPerCentimeterResolution;
            image->x_resolution/=2.54;
            image->y_resolution/=2.54;
          }
        break;
      }
      case 0x0404:
      {
        /*
          IPTC Profile
        */
        profile=AcquireStringInfo(count);
        SetStringInfoDatum(profile,p);
        (void) SetImageProfileInternal(image,"iptc",profile,MagickTrue);
        profile=DestroyStringInfo(profile);
        p+=count;
        break;
      }
      case 0x040c:
      {
        /*
          Thumbnail.
        */
        p+=count;
        break;
      }
      case 0x040f:
      {
        /*
          ICC Profile.
        */
        profile=AcquireStringInfo(count);
        SetStringInfoDatum(profile,p);
        (void) SetImageProfileInternal(image,"icc",profile,MagickTrue);
        profile=DestroyStringInfo(profile);
        p+=count;
        break;
      }
      case 0x0422:
      {
        /*
          EXIF Profile.
        */
        profile=AcquireStringInfo(count);
        SetStringInfoDatum(profile,p);
        (void) SetImageProfileInternal(image,"exif",profile,MagickTrue);
        profile=DestroyStringInfo(profile);
        p+=count;
        break;
      }
      case 0x0424:
      {
        /*
          XMP Profile.
        */
        profile=AcquireStringInfo(count);
        SetStringInfoDatum(profile,p);
        (void) SetImageProfileInternal(image,"xmp",profile,MagickTrue);
        profile=DestroyStringInfo(profile);
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
}
