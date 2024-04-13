MagickExport size_t InterpretImageFilename(const ImageInfo *image_info,
  Image *image,const char *format,int value,char *filename)
{
  char
    *q;

  int
    c;

  MagickBooleanType
    canonical;

  register const char
    *p;

  size_t
    length;

  canonical=MagickFalse;
  length=0;
  (void) CopyMagickString(filename,format,MaxTextExtent);
  for (p=strchr(format,'%'); p != (char *) NULL; p=strchr(p+1,'%'))
  {
    q=(char *) p+1;
    if (*q == '%')
      {
        p=q+1;
        continue;
      }
    if (*q == '0')
      {
        ssize_t
          value;

        value=(ssize_t) strtol(q,&q,10);
        (void) value;
      }
    switch (*q)
    {
      case 'd':
      case 'o':
      case 'x':
      {
        q++;
        c=(*q);
        *q='\0';
        (void) FormatLocaleString(filename+(p-format),(size_t) (MaxTextExtent-
          (p-format)),p,value);
        *q=c;
        (void) ConcatenateMagickString(filename,q,MaxTextExtent);
        canonical=MagickTrue;
        if (*(q-1) != '%')
          break;
        p++;
        break;
      }
      case '[':
      {
        char
          pattern[MaxTextExtent];

        const char
          *value;

        register char
          *r;

        register ssize_t
          i;

        ssize_t
          depth;

        /*
          Image option.
        */
        if (strchr(p,']') == (char *) NULL)
          break;
        depth=1;
        r=q+1;
        for (i=0; (i < (MaxTextExtent-1L)) && (*r != '\0'); i++)
        {
          if (*r == '[')
            depth++;
          if (*r == ']')
            depth--;
          if (depth <= 0)
            break;
          pattern[i]=(*r++);
        }
        pattern[i]='\0';
        if (LocaleNCompare(pattern,"filename:",9) != 0)
          break;
        value=(const char *) NULL;
#if 0
        /* FUTURE: remove this code. -- Anthony  29 Arpil 2012
           Removed as GetMagickProperty() will will never match a "filename:"
           string as this is not a 'known' image property.
        */
        if ((image_info != (const ImageInfo *) NULL) &&
            (image != (const Image *) NULL))
          value=GetMagickProperty(image_info,image,pattern);
        else
#endif
        if (image != (Image *) NULL)
          value=GetImageProperty(image,pattern);
        if ((value == (const char *) NULL) &&
            (image != (Image *) NULL))
          value=GetImageArtifact(image,pattern);
        if ((value == (const char *) NULL) &&
            (image_info != (ImageInfo *) NULL))
          value=GetImageOption(image_info,pattern);
        if (value == (const char *) NULL)
          break;
        q--;
        c=(*q);
        *q='\0';
        (void) CopyMagickString(filename+(p-format-length),value,(size_t)
          (MaxTextExtent-(p-format-length)));
        length+=strlen(pattern)-1;
        *q=c;
        (void) ConcatenateMagickString(filename,r+1,MaxTextExtent);
        canonical=MagickTrue;
        if (*(q-1) != '%')
          break;
        p++;
        break;
      }
      default:
        break;
    }
  }
  for (q=filename; *q != '\0'; q++)
    if ((*q == '%') && (*(q+1) == '%'))
      {
        (void) CopyMagickString(q,q+1,(size_t) (MaxTextExtent-(q-filename)));
        canonical=MagickTrue;
      }
  if (canonical == MagickFalse)
    (void) CopyMagickString(filename,format,MaxTextExtent);
  return(strlen(filename));
}
