ModuleExport size_t RegisterTIFFImage(void)
{
#define TIFFDescription  "Tagged Image File Format"

  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  if (tiff_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&tiff_semaphore);
  LockSemaphoreInfo(tiff_semaphore);
  if (instantiate_key == MagickFalse)
    {
      if (CreateMagickThreadKey(&tiff_exception,NULL) == MagickFalse)
        ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
      error_handler=TIFFSetErrorHandler(TIFFErrors);
      warning_handler=TIFFSetWarningHandler(TIFFWarnings);
#if defined(MAGICKCORE_HAVE_TIFFMERGEFIELDINFO) && defined(MAGICKCORE_HAVE_TIFFSETTAGEXTENDER)
      if (tag_extender == (TIFFExtendProc) NULL)
        tag_extender=TIFFSetTagExtender(TIFFTagExtender);
#endif
      instantiate_key=MagickTrue;
    }
  UnlockSemaphoreInfo(tiff_semaphore);
  *version='\0';
#if defined(TIFF_VERSION)
  (void) FormatLocaleString(version,MaxTextExtent,"%d",TIFF_VERSION);
#endif
#if defined(MAGICKCORE_TIFF_DELEGATE)
  {
    const char
      *p;

    register ssize_t
      i;

    p=TIFFGetVersion();
    for (i=0; (i < (MaxTextExtent-1)) && (*p != 0) && (*p != '\n'); i++)
      version[i]=(*p++);
    version[i]='\0';
  }
#endif

  entry=SetMagickInfo("GROUP4");
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadGROUP4Image;
  entry->encoder=(EncodeImageHandler *) WriteGROUP4Image;
#endif
  entry->raw=MagickTrue;
  entry->endian_support=MagickTrue;
  entry->adjoin=MagickFalse;
  entry->format_type=ImplicitFormatType;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Raw CCITT Group4");
  entry->mime_type=ConstantString("image/tiff");
  entry->module=ConstantString("TIFF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PTIF");
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadTIFFImage;
  entry->encoder=(EncodeImageHandler *) WritePTIFImage;
#endif
  entry->endian_support=MagickTrue;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Pyramid encoded TIFF");
  entry->mime_type=ConstantString("image/tiff");
  entry->module=ConstantString("TIFF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("TIF");
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadTIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteTIFFImage;
#endif
  entry->endian_support=MagickTrue;
  entry->seekable_stream=MagickTrue;
  entry->stealth=MagickTrue;
  entry->description=ConstantString(TIFFDescription);
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/tiff");
  entry->module=ConstantString("TIFF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("TIFF");
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadTIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteTIFFImage;
#endif
  entry->magick=(IsImageFormatHandler *) IsTIFF;
  entry->endian_support=MagickTrue;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString(TIFFDescription);
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/tiff");
  entry->module=ConstantString("TIFF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("TIFF64");
#if defined(TIFF_VERSION_BIG)
  entry->decoder=(DecodeImageHandler *) ReadTIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteTIFFImage;
#endif
  entry->adjoin=MagickFalse;
  entry->endian_support=MagickTrue;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Tagged Image File Format (64-bit)");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/tiff");
  entry->module=ConstantString("TIFF");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
