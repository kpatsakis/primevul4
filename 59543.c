static void TIFFGetProperties(TIFF *tiff,Image *image)
{
  char
    message[MaxTextExtent],
    *text;

  uint32
    count,
    length,
    type;

  unsigned long
    *tietz;

  if ((TIFFGetField(tiff,TIFFTAG_ARTIST,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:artist",text);
  if ((TIFFGetField(tiff,TIFFTAG_COPYRIGHT,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:copyright",text);
  if ((TIFFGetField(tiff,TIFFTAG_DATETIME,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:timestamp",text);
  if ((TIFFGetField(tiff,TIFFTAG_DOCUMENTNAME,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:document",text);
  if ((TIFFGetField(tiff,TIFFTAG_HOSTCOMPUTER,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:hostcomputer",text);
  if ((TIFFGetField(tiff,TIFFTAG_IMAGEDESCRIPTION,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"comment",text);
  if ((TIFFGetField(tiff,TIFFTAG_MAKE,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:make",text);
  if ((TIFFGetField(tiff,TIFFTAG_MODEL,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:model",text);
  if ((TIFFGetField(tiff,TIFFTAG_OPIIMAGEID,&count,&text) == 1) &&
      (text != (char *) NULL))
    {
      if (count >= MaxTextExtent)
        count=MaxTextExtent-1;
      (void) CopyMagickString(message,text,count+1);
      (void) SetImageProperty(image,"tiff:image-id",message);
    }
  if ((TIFFGetField(tiff,TIFFTAG_PAGENAME,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"label",text);
  if ((TIFFGetField(tiff,TIFFTAG_SOFTWARE,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:software",text);
  if ((TIFFGetField(tiff,33423,&count,&text) == 1) &&
      (text != (char *) NULL))
    {
      if (count >= MaxTextExtent)
        count=MaxTextExtent-1;
      (void) CopyMagickString(message,text,count+1);
      (void) SetImageProperty(image,"tiff:kodak-33423",message);
    }
  if ((TIFFGetField(tiff,36867,&count,&text) == 1) &&
      (text != (char *) NULL))
    {
      if (count >= MaxTextExtent)
        count=MaxTextExtent-1;
      (void) CopyMagickString(message,text,count+1);
      (void) SetImageProperty(image,"tiff:kodak-36867",message);
    }
  if (TIFFGetField(tiff,TIFFTAG_SUBFILETYPE,&type) == 1)
    switch (type)
    {
      case 0x01:
      {
        (void) SetImageProperty(image,"tiff:subfiletype","REDUCEDIMAGE");
        break;
      }
      case 0x02:
      {
        (void) SetImageProperty(image,"tiff:subfiletype","PAGE");
        break;
      }
      case 0x04:
      {
        (void) SetImageProperty(image,"tiff:subfiletype","MASK");
        break;
      }
      default:
        break;
    }
  if ((TIFFGetField(tiff,37706,&length,&tietz) == 1) &&
      (tietz != (unsigned long *) NULL))
    {
      (void) FormatLocaleString(message,MaxTextExtent,"%lu",tietz[0]);
      (void) SetImageProperty(image,"tiff:tietz_offset",message);
    }
}
