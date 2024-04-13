ModuleExport size_t RegisterPNGImage(void)
{
  char
    version[MagickPathExtent];

  MagickInfo
    *entry;

  static const char
    *PNGNote=
    {
      "See http://www.libpng.org/ for details about the PNG format."
    },

    *JNGNote=
    {
      "See http://www.libpng.org/pub/mng/ for details about the JNG\n"
      "format."
    },

    *MNGNote=
    {
      "See http://www.libpng.org/pub/mng/ for details about the MNG\n"
      "format."
    };

  *version='\0';

#if defined(PNG_LIBPNG_VER_STRING)
  (void) ConcatenateMagickString(version,"libpng ",MagickPathExtent);
  (void) ConcatenateMagickString(version,PNG_LIBPNG_VER_STRING,
   MagickPathExtent);

  if (LocaleCompare(PNG_LIBPNG_VER_STRING,png_get_header_ver(NULL)) != 0)
    {
      (void) ConcatenateMagickString(version,",",MagickPathExtent);
      (void) ConcatenateMagickString(version,png_get_libpng_ver(NULL),
            MagickPathExtent);
    }
#endif

  entry=AcquireMagickInfo("PNG","MNG","Multiple-image Network Graphics");
  entry->flags|=CoderDecoderSeekableStreamFlag;

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadMNGImage;
  entry->encoder=(EncodeImageHandler *) WriteMNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsMNG;

  if (*version != '\0')
    entry->version=ConstantString(version);

  entry->mime_type=ConstantString("video/x-mng");
  entry->note=ConstantString(MNGNote);
  (void) RegisterMagickInfo(entry);

  entry=AcquireMagickInfo("PNG","PNG","Portable Network Graphics");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->mime_type=ConstantString("image/png");

  if (*version != '\0')
    entry->version=ConstantString(version);

  entry->note=ConstantString(PNGNote);
  (void) RegisterMagickInfo(entry);

  entry=AcquireMagickInfo("PNG","PNG8",
    "8-bit indexed with optional binary transparency");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->mime_type=ConstantString("image/png");
  (void) RegisterMagickInfo(entry);

  entry=AcquireMagickInfo("PNG","PNG24",
    "opaque or binary transparent 24-bit RGB");
  *version='\0';

#if defined(ZLIB_VERSION)
  (void) ConcatenateMagickString(version,"zlib ",MagickPathExtent);
  (void) ConcatenateMagickString(version,ZLIB_VERSION,MagickPathExtent);

  if (LocaleCompare(ZLIB_VERSION,zlib_version) != 0)
    {
      (void) ConcatenateMagickString(version,",",MagickPathExtent);
      (void) ConcatenateMagickString(version,zlib_version,MagickPathExtent);
    }
#endif

  if (*version != '\0')
    entry->version=ConstantString(version);

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->mime_type=ConstantString("image/png");
  (void) RegisterMagickInfo(entry);

  entry=AcquireMagickInfo("PNG","PNG32","opaque or transparent 32-bit RGBA");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->mime_type=ConstantString("image/png");
  (void) RegisterMagickInfo(entry);

  entry=AcquireMagickInfo("PNG","PNG48",
    "opaque or binary transparent 48-bit RGB");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->mime_type=ConstantString("image/png");
  (void) RegisterMagickInfo(entry);

  entry=AcquireMagickInfo("PNG","PNG64","opaque or transparent 64-bit RGBA");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->mime_type=ConstantString("image/png");
  (void) RegisterMagickInfo(entry);

  entry=AcquireMagickInfo("PNG","PNG00",
    "PNG inheriting bit-depth, color-type from original, if possible");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->mime_type=ConstantString("image/png");
  (void) RegisterMagickInfo(entry);

  entry=AcquireMagickInfo("PNG","JNG","JPEG Network Graphics");

#if defined(JNG_SUPPORTED)
#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJNGImage;
  entry->encoder=(EncodeImageHandler *) WriteJNGImage;
#endif
#endif

  entry->magick=(IsImageFormatHandler *) IsJNG;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->mime_type=ConstantString("image/x-jng");
  entry->note=ConstantString(JNGNote);
  (void) RegisterMagickInfo(entry);

#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
  ping_semaphore=AcquireSemaphoreInfo();
#endif

  return(MagickImageCoderSignature);
}
