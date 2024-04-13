MagickExport MagickBooleanType SetImageInfo(ImageInfo *image_info,
  const unsigned int frames,ExceptionInfo *exception)
{
  char
    extension[MaxTextExtent],
    filename[MaxTextExtent],
    magic[MaxTextExtent],
    *q,
    subimage[MaxTextExtent];

  const MagicInfo
    *magic_info;

  const MagickInfo
    *magick_info;

  ExceptionInfo
    *sans_exception;

  Image
    *image;

  MagickBooleanType
    status;

  register const char
    *p;

  ssize_t
    count;

  unsigned char
    magick[2*MaxTextExtent];

  /*
    Look for 'image.format' in filename.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  *subimage='\0';
  GetPathComponent(image_info->filename,SubimagePath,subimage);
  if (*subimage != '\0')
    {
      /*
        Look for scene specification (e.g. img0001.pcd[4]).
      */
      if (IsSceneGeometry(subimage,MagickFalse) == MagickFalse)
        {
          if (IsGeometry(subimage) != MagickFalse)
            (void) CloneString(&image_info->extract,subimage);
        }
      else
        {
          size_t
            first,
            last;

          (void) CloneString(&image_info->scenes,subimage);
          image_info->scene=StringToUnsignedLong(image_info->scenes);
          image_info->number_scenes=image_info->scene;
          p=image_info->scenes;
          for (q=(char *) image_info->scenes; *q != '\0'; p++)
          {
            while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
              p++;
            first=(size_t) strtol(p,&q,10);
            last=first;
            while (isspace((int) ((unsigned char) *q)) != 0)
              q++;
            if (*q == '-')
              last=(size_t) strtol(q+1,&q,10);
            if (first > last)
              Swap(first,last);
            if (first < image_info->scene)
              image_info->scene=first;
            if (last > image_info->number_scenes)
              image_info->number_scenes=last;
            p=q;
          }
          image_info->number_scenes-=image_info->scene-1;
          image_info->subimage=image_info->scene;
          image_info->subrange=image_info->number_scenes;
        }
    }
  *extension='\0';
  if (*image_info->magick == '\0')
    GetPathComponent(image_info->filename,ExtensionPath,extension);
#if defined(MAGICKCORE_ZLIB_DELEGATE)
  if (*extension != '\0')
    if ((LocaleCompare(extension,"gz") == 0) ||
        (LocaleCompare(extension,"Z") == 0) ||
        (LocaleCompare(extension,"svgz") == 0) ||
        (LocaleCompare(extension,"wmz") == 0))
      {
        char
          path[MaxTextExtent];

        (void) CopyMagickString(path,image_info->filename,MaxTextExtent);
        path[strlen(path)-strlen(extension)-1]='\0';
        GetPathComponent(path,ExtensionPath,extension);
      }
#endif
#if defined(MAGICKCORE_BZLIB_DELEGATE)
  if (*extension != '\0')
    if (LocaleCompare(extension,"bz2") == 0)
      {
        char
          path[MaxTextExtent];

        (void) CopyMagickString(path,image_info->filename,MaxTextExtent);
        path[strlen(path)-strlen(extension)-1]='\0';
        GetPathComponent(path,ExtensionPath,extension);
      }
#endif
  image_info->affirm=MagickFalse;
  sans_exception=AcquireExceptionInfo();
  if (*extension != '\0')
    {
      MagickFormatType
        format_type;

      register ssize_t
        i;

      static const char
        *format_type_formats[] =
        {
          "AUTOTRACE",
          "BROWSE",
          "DCRAW",
          "EDIT",
          "LAUNCH",
          "MPEG:DECODE",
          "MPEG:ENCODE",
          "PRINT",
          "PS:ALPHA",
          "PS:CMYK",
          "PS:COLOR",
          "PS:GRAY",
          "PS:MONO",
          "SCAN",
          "SHOW",
          "WIN",
          (char *) NULL
        };

      /*
        User specified image format.
      */
      (void) CopyMagickString(magic,extension,MaxTextExtent);
      LocaleUpper(magic);
      /*
        Look for explicit image formats.
      */
      format_type=UndefinedFormatType;
      i=0;
      while ((format_type == UndefinedFormatType) &&
             (format_type_formats[i] != (char *) NULL))
      {
        if ((*magic == *format_type_formats[i]) &&
            (LocaleCompare(magic,format_type_formats[i]) == 0))
          format_type=ExplicitFormatType;
        i++;
      }
      magick_info=GetMagickInfo(magic,sans_exception);
      if ((magick_info != (const MagickInfo *) NULL) &&
          (magick_info->format_type != UndefinedFormatType))
        format_type=magick_info->format_type;
      if (format_type == UndefinedFormatType)
        (void) CopyMagickString(image_info->magick,magic,MaxTextExtent);
      else
        if (format_type == ExplicitFormatType)
          {
            image_info->affirm=MagickTrue;
            (void) CopyMagickString(image_info->magick,magic,MaxTextExtent);
          }
      if (LocaleCompare(magic,"RGB") == 0)
        image_info->affirm=MagickFalse;  /* maybe SGI disguised as RGB */
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  *magic='\0';
  GetPathComponent(image_info->filename,MagickPath,magic);
  if (*magic == '\0')
    {
      (void) CopyMagickString(magic,image_info->magick,MaxTextExtent);
      magick_info=GetMagickInfo(magic,sans_exception);
      GetPathComponent(image_info->filename,CanonicalPath,filename);
      (void) CopyMagickString(image_info->filename,filename,MaxTextExtent);
    }
  else
    {
      const DelegateInfo
        *delegate_info;

      /*
        User specified image format.
      */
      LocaleUpper(magic);
      magick_info=GetMagickInfo(magic,sans_exception);
      delegate_info=GetDelegateInfo(magic,"*",sans_exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        delegate_info=GetDelegateInfo("*",magic,sans_exception);
      if (((magick_info != (const MagickInfo *) NULL) ||
           (delegate_info != (const DelegateInfo *) NULL)) &&
          (IsMagickConflict(magic) == MagickFalse))
        {
          image_info->affirm=MagickTrue;
          (void) CopyMagickString(image_info->magick,magic,MaxTextExtent);
          GetPathComponent(image_info->filename,CanonicalPath,filename);
          (void) CopyMagickString(image_info->filename,filename,MaxTextExtent);
        }
    }
  sans_exception=DestroyExceptionInfo(sans_exception);
  if ((magick_info == (const MagickInfo *) NULL) ||
      (GetMagickEndianSupport(magick_info) == MagickFalse))
    image_info->endian=UndefinedEndian;
  if ((image_info->adjoin != MagickFalse) && (frames > 1))
    {
      /*
        Test for multiple image support (e.g. image%02d.png).
      */
      (void) InterpretImageFilename(image_info,(Image *) NULL,
        image_info->filename,(int) image_info->scene,filename);
      if ((LocaleCompare(filename,image_info->filename) != 0) &&
          (strchr(filename,'%') == (char *) NULL))
        image_info->adjoin=MagickFalse;
    }
  if ((image_info->adjoin != MagickFalse) && (frames > 0))
    {
      /*
        Some image formats do not support multiple frames per file.
      */
      magick_info=GetMagickInfo(magic,exception);
      if (magick_info != (const MagickInfo *) NULL)
        if (GetMagickAdjoin(magick_info) == MagickFalse)
          image_info->adjoin=MagickFalse;
    }
  if (image_info->affirm != MagickFalse)
    return(MagickTrue);
  if (frames == 0)
    {
      /*
        Determine the image format from the first few bytes of the file.
      */
      image=AcquireImage(image_info);
      (void) CopyMagickString(image->filename,image_info->filename,
        MaxTextExtent);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == MagickFalse)
        {
          image=DestroyImage(image);
          return(MagickFalse);
        }
      if ((IsBlobSeekable(image) == MagickFalse) ||
          (IsBlobExempt(image) != MagickFalse))
        {
          /*
            Copy standard input or pipe to temporary file.
          */
          *filename='\0';
          status=ImageToFile(image,filename,exception);
          (void) CloseBlob(image);
          if (status == MagickFalse)
            {
              image=DestroyImage(image);
              return(MagickFalse);
            }
          SetImageInfoFile(image_info,(FILE *) NULL);
          (void) CopyMagickString(image->filename,filename,MaxTextExtent);
          status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
          if (status == MagickFalse)
            {
              image=DestroyImage(image);
              return(MagickFalse);
            }
          (void) CopyMagickString(image_info->filename,filename,MaxTextExtent);
          image_info->temporary=MagickTrue;
        }
      (void) ResetMagickMemory(magick,0,sizeof(magick));
      count=ReadBlob(image,2*MaxTextExtent,magick);
      (void) SeekBlob(image,-((MagickOffsetType) count),SEEK_CUR);
      (void) CloseBlob(image);
      image=DestroyImage(image);
      /*
        Check magic.xml configuration file.
      */
      sans_exception=AcquireExceptionInfo();
      magic_info=GetMagicInfo(magick,(size_t) count,sans_exception);
      if ((magic_info != (const MagicInfo *) NULL) &&
          (GetMagicName(magic_info) != (char *) NULL))
        {
          (void) CopyMagickString(image_info->magick,GetMagicName(magic_info),
            MaxTextExtent);
          magick_info=GetMagickInfo(image_info->magick,sans_exception);
          if ((magick_info == (const MagickInfo *) NULL) ||
              (GetMagickEndianSupport(magick_info) == MagickFalse))
            image_info->endian=UndefinedEndian;
          sans_exception=DestroyExceptionInfo(sans_exception);
          return(MagickTrue);
        }
      magick_info=GetMagickInfo(image_info->magick,sans_exception);
      if ((magick_info == (const MagickInfo *) NULL) ||
          (GetMagickEndianSupport(magick_info) == MagickFalse))
        image_info->endian=UndefinedEndian;
      sans_exception=DestroyExceptionInfo(sans_exception);
    }
  return(MagickTrue);
}
