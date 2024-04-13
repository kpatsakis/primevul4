static const StringInfo *GetAdditionalInformation(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
#define PSDKeySize 5
#define PSDAllowedLength 36

  char
    key[PSDKeySize];

  /* Whitelist of keys from: https://www.adobe.com/devnet-apps/photoshop/fileformatashtml/ */
  const char
    allowed[PSDAllowedLength][PSDKeySize] = {
      "blnc", "blwh", "brit", "brst", "clbl", "clrL", "curv", "expA", "FMsk",
      "GdFl", "grdm", "hue ", "hue2", "infx", "knko", "lclr", "levl", "lnsr",
      "lfx2", "luni", "lrFX", "lspf", "lyid", "lyvr", "mixr", "nvrt", "phfl",
      "post", "PtFl", "selc", "shpa", "sn2P", "SoCo", "thrs", "tsly", "vibA"
    },
    *option;

  const StringInfo
    *info;

  MagickBooleanType
    found;

  register size_t
    i;

  size_t
    remaining_length,
    length;

  StringInfo
    *profile;

  unsigned char
    *p;

  unsigned int
    size;

  info=GetImageProfile(image,"psd:additional-info");
  if (info == (const StringInfo *) NULL)
    return((const StringInfo *) NULL);
  option=GetImageOption(image_info,"psd:additional-info");
  if (LocaleCompare(option,"all") == 0)
    return(info);
  if (LocaleCompare(option,"selective") != 0)
    {
      profile=RemoveImageProfile(image,"psd:additional-info");
      return(DestroyStringInfo(profile));
    }
  length=GetStringInfoLength(info);
  p=GetStringInfoDatum(info);
  remaining_length=length;
  length=0;
  while (remaining_length >= 12)
  {
    /* skip over signature */
    p+=4;
    key[0]=(*p++);
    key[1]=(*p++);
    key[2]=(*p++);
    key[3]=(*p++);
    key[4]='\0';
    size=(unsigned int) (*p++) << 24;
    size|=(unsigned int) (*p++) << 16;
    size|=(unsigned int) (*p++) << 8;
    size|=(unsigned int) (*p++);
    size=size & 0xffffffff;
    remaining_length-=12;
    if ((size_t) size > remaining_length)
      return((const StringInfo *) NULL);
    found=MagickFalse;
    for (i=0; i < PSDAllowedLength; i++)
    {
      if (LocaleNCompare(key,allowed[i],PSDKeySize) != 0)
        continue;

      found=MagickTrue;
      break;
    }
    remaining_length-=(size_t) size;
    if (found == MagickFalse)
      {
        if (remaining_length > 0)
          p=(unsigned char *) CopyMagickMemory(p-12,p+size,remaining_length);
        continue;
      }
    length+=(size_t) size+12;
    p+=size;
  }
  profile=RemoveImageProfile(image,"psd:additional-info");
  if (length == 0)
    return(DestroyStringInfo(profile));
  SetStringInfoLength(profile,(const size_t) length);
  SetImageProfile(image,"psd:additional-info",info,exception);
  return(profile);
}
