MagickExport MagickBooleanType ProfileImage(Image *image,const char *name,
  const void *datum,const size_t length,
  const MagickBooleanType magick_unused(clone))
{
#define ProfileImageTag  "Profile/Image"
#define ThrowProfileException(severity,tag,context) \
{ \
  if (source_profile != (cmsHPROFILE) NULL) \
    (void) cmsCloseProfile(source_profile); \
  if (target_profile != (cmsHPROFILE) NULL) \
    (void) cmsCloseProfile(target_profile); \
  ThrowBinaryException(severity,tag,context); \
}

  MagickBooleanType
    status;

  StringInfo
    *profile;

  magick_unreferenced(clone);

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(name != (const char *) NULL);
  if ((datum == (const void *) NULL) || (length == 0))
    {
      char
        *next;

      /*
        Delete image profile(s).
      */
      ResetImageProfileIterator(image);
      for (next=GetNextImageProfile(image); next != (const char *) NULL; )
      {
        if (IsOptionMember(next,name) != MagickFalse)
          {
            (void) DeleteImageProfile(image,next);
            ResetImageProfileIterator(image);
          }
        next=GetNextImageProfile(image);
      }
      return(MagickTrue);
    }
  /*
    Add a ICC, IPTC, or generic profile to the image.
  */
  status=MagickTrue;
  profile=AcquireStringInfo((size_t) length);
  SetStringInfoDatum(profile,(unsigned char *) datum);
  if ((LocaleCompare(name,"icc") != 0) && (LocaleCompare(name,"icm") != 0))
    status=SetImageProfile(image,name,profile);
  else
    {
      const StringInfo
        *icc_profile;

      icc_profile=GetImageProfile(image,"icc");
      if ((icc_profile != (const StringInfo *) NULL) &&
          (CompareStringInfo(icc_profile,profile) == 0))
        {
          const char
            *value;

          value=GetImageProperty(image,"exif:ColorSpace");
          (void) value;
          if (LocaleCompare(value,"1") != 0)
            (void) SetsRGBImageProfile(image);
          value=GetImageProperty(image,"exif:InteroperabilityIndex");
          if (LocaleCompare(value,"R98.") != 0)
            (void) SetsRGBImageProfile(image);
          /* Future.
          value=GetImageProperty(image,"exif:InteroperabilityIndex");
          if (LocaleCompare(value,"R03.") != 0)
            (void) SetAdobeRGB1998ImageProfile(image);
          */
          icc_profile=GetImageProfile(image,"icc");
        }
      if ((icc_profile != (const StringInfo *) NULL) &&
          (CompareStringInfo(icc_profile,profile) == 0))
        {
          profile=DestroyStringInfo(profile);
          return(MagickTrue);
        }
#if !defined(MAGICKCORE_LCMS_DELEGATE)
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        MissingDelegateWarning,"DelegateLibrarySupportNotBuiltIn","`%s' (LCMS)",
        image->filename);
#else
      {
        cmsHPROFILE
          source_profile;

        /*
          Transform pixel colors as defined by the color profiles.
        */
        cmsSetLogErrorHandler(LCMSExceptionHandler);
        source_profile=cmsOpenProfileFromMemTHR((cmsContext) image,
          GetStringInfoDatum(profile),(cmsUInt32Number)
          GetStringInfoLength(profile));
        if (source_profile == (cmsHPROFILE) NULL)
          ThrowBinaryException(ResourceLimitError,
            "ColorspaceColorProfileMismatch",name);
        if ((cmsGetDeviceClass(source_profile) != cmsSigLinkClass) &&
            (icc_profile == (StringInfo *) NULL))
          status=SetImageProfile(image,name,profile);
        else
          {
            CacheView
              *image_view;

            ColorspaceType
              source_colorspace,
              target_colorspace;

            cmsColorSpaceSignature
              signature;

            cmsHPROFILE
              target_profile;

            cmsHTRANSFORM
              *magick_restrict transform;

            cmsUInt32Number
              flags,
              source_type,
              target_type;

            ExceptionInfo
              *exception;

            int
              intent;

            MagickOffsetType
              progress;

            size_t
              source_channels,
              target_channels;

            ssize_t
              y;

            unsigned short
              **magick_restrict source_pixels,
              **magick_restrict target_pixels;

            exception=(&image->exception);
            target_profile=(cmsHPROFILE) NULL;
            if (icc_profile != (StringInfo *) NULL)
              {
                target_profile=source_profile;
                source_profile=cmsOpenProfileFromMemTHR((cmsContext) image,
                  GetStringInfoDatum(icc_profile),(cmsUInt32Number)
                  GetStringInfoLength(icc_profile));
                if (source_profile == (cmsHPROFILE) NULL)
                  ThrowProfileException(ResourceLimitError,
                    "ColorspaceColorProfileMismatch",name);
              }
            switch (cmsGetColorSpace(source_profile))
            {
              case cmsSigCmykData:
              {
                source_colorspace=CMYKColorspace;
                source_type=(cmsUInt32Number) TYPE_CMYK_16;
                source_channels=4;
                break;
              }
              case cmsSigGrayData:
              {
                source_colorspace=GRAYColorspace;
                source_type=(cmsUInt32Number) TYPE_GRAY_16;
                source_channels=1;
                break;
              }
              case cmsSigLabData:
              {
                source_colorspace=LabColorspace;
                source_type=(cmsUInt32Number) TYPE_Lab_16;
                source_channels=3;
                break;
              }
              case cmsSigLuvData:
              {
                source_colorspace=YUVColorspace;
                source_type=(cmsUInt32Number) TYPE_YUV_16;
                source_channels=3;
                break;
              }
              case cmsSigRgbData:
              {
                source_colorspace=sRGBColorspace;
                source_type=(cmsUInt32Number) TYPE_RGB_16;
                source_channels=3;
                break;
              }
              case cmsSigXYZData:
              {
                source_colorspace=XYZColorspace;
                source_type=(cmsUInt32Number) TYPE_XYZ_16;
                source_channels=3;
                break;
              }
              case cmsSigYCbCrData:
              {
                source_colorspace=YCbCrColorspace;
                source_type=(cmsUInt32Number) TYPE_YCbCr_16;
                source_channels=3;
                break;
              }
              default:
              {
                source_colorspace=UndefinedColorspace;
                source_type=(cmsUInt32Number) TYPE_RGB_16;
                source_channels=3;
                break;
              }
            }
            signature=cmsGetPCS(source_profile);
            if (target_profile != (cmsHPROFILE) NULL)
              signature=cmsGetColorSpace(target_profile);
            switch (signature)
            {
              case cmsSigCmykData:
              {
                target_colorspace=CMYKColorspace;
                target_type=(cmsUInt32Number) TYPE_CMYK_16;
                target_channels=4;
                break;
              }
              case cmsSigLabData:
              {
                target_colorspace=LabColorspace;
                target_type=(cmsUInt32Number) TYPE_Lab_16;
                target_channels=3;
                break;
              }
              case cmsSigGrayData:
              {
                target_colorspace=GRAYColorspace;
                target_type=(cmsUInt32Number) TYPE_GRAY_16;
                target_channels=1;
                break;
              }
              case cmsSigLuvData:
              {
                target_colorspace=YUVColorspace;
                target_type=(cmsUInt32Number) TYPE_YUV_16;
                target_channels=3;
                break;
              }
              case cmsSigRgbData:
              {
                target_colorspace=sRGBColorspace;
                target_type=(cmsUInt32Number) TYPE_RGB_16;
                target_channels=3;
                break;
              }
              case cmsSigXYZData:
              {
                target_colorspace=XYZColorspace;
                target_type=(cmsUInt32Number) TYPE_XYZ_16;
                target_channels=3;
                break;
              }
              case cmsSigYCbCrData:
              {
                target_colorspace=YCbCrColorspace;
                target_type=(cmsUInt32Number) TYPE_YCbCr_16;
                target_channels=3;
                break;
              }
              default:
              {
                target_colorspace=UndefinedColorspace;
                target_type=(cmsUInt32Number) TYPE_RGB_16;
                target_channels=3;
                break;
              }
            }
            if ((source_colorspace == UndefinedColorspace) ||
                (target_colorspace == UndefinedColorspace))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace == GRAYColorspace) &&
                 (SetImageGray(image,exception) == MagickFalse))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace == CMYKColorspace) &&
                 (image->colorspace != CMYKColorspace))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace == XYZColorspace) &&
                 (image->colorspace != XYZColorspace))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace == YCbCrColorspace) &&
                 (image->colorspace != YCbCrColorspace))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace != CMYKColorspace) &&
                 (source_colorspace != GRAYColorspace) &&
                 (source_colorspace != LabColorspace) &&
                 (source_colorspace != XYZColorspace) &&
                 (source_colorspace != YCbCrColorspace) &&
                 (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
            switch (image->rendering_intent)
            {
              case AbsoluteIntent: intent=INTENT_ABSOLUTE_COLORIMETRIC; break;
              case PerceptualIntent: intent=INTENT_PERCEPTUAL; break;
              case RelativeIntent: intent=INTENT_RELATIVE_COLORIMETRIC; break;
              case SaturationIntent: intent=INTENT_SATURATION; break;
              default: intent=INTENT_PERCEPTUAL; break;
            }
            flags=cmsFLAGS_HIGHRESPRECALC;
#if defined(cmsFLAGS_BLACKPOINTCOMPENSATION)
            if (image->black_point_compensation != MagickFalse)
              flags|=cmsFLAGS_BLACKPOINTCOMPENSATION;
#endif
            transform=AcquireTransformThreadSet(image,source_profile,
              source_type,target_profile,target_type,intent,flags);
            if (transform == (cmsHTRANSFORM *) NULL)
              ThrowProfileException(ImageError,"UnableToCreateColorTransform",
                name);
            /*
              Transform image as dictated by the source & target image profiles.
            */
            source_pixels=AcquirePixelThreadSet(image->columns,source_channels);
            target_pixels=AcquirePixelThreadSet(image->columns,target_channels);
            if ((source_pixels == (unsigned short **) NULL) ||
                (target_pixels == (unsigned short **) NULL))
              {
                transform=DestroyTransformThreadSet(transform);
                ThrowProfileException(ResourceLimitError,
                  "MemoryAllocationFailed",image->filename);
              }
            if (SetImageStorageClass(image,DirectClass) == MagickFalse)
              {
                target_pixels=DestroyPixelThreadSet(target_pixels);
                source_pixels=DestroyPixelThreadSet(source_pixels);
                transform=DestroyTransformThreadSet(transform);
                if (source_profile != (cmsHPROFILE) NULL)
                  (void) cmsCloseProfile(source_profile);
                if (target_profile != (cmsHPROFILE) NULL)
                  (void) cmsCloseProfile(target_profile);
                return(MagickFalse);
              }
            if (target_colorspace == CMYKColorspace)
              (void) SetImageColorspace(image,target_colorspace);
            progress=0;
            image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
            #pragma omp parallel for schedule(static,4) shared(status) \
              magick_threads(image,image,image->rows,1)
#endif
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              const int
                id = GetOpenMPThreadId();

              MagickBooleanType
                sync;

              register IndexPacket
                *magick_restrict indexes;

              register ssize_t
                x;

              register PixelPacket
                *magick_restrict q;

              register unsigned short
                *p;

              if (status == MagickFalse)
                continue;
              q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
                exception);
              if (q == (PixelPacket *) NULL)
                {
                  status=MagickFalse;
                  continue;
                }
              indexes=GetCacheViewAuthenticIndexQueue(image_view);
              p=source_pixels[id];
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                *p++=ScaleQuantumToShort(GetPixelRed(q));
                if (source_channels > 1)
                  {
                    *p++=ScaleQuantumToShort(GetPixelGreen(q));
                    *p++=ScaleQuantumToShort(GetPixelBlue(q));
                  }
                if (source_channels > 3)
                  *p++=ScaleQuantumToShort(GetPixelIndex(indexes+x));
                q++;
              }
              cmsDoTransform(transform[id],source_pixels[id],target_pixels[id],
                (unsigned int) image->columns);
              p=target_pixels[id];
              q-=image->columns;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelRed(q,ScaleShortToQuantum(*p));
                SetPixelGreen(q,GetPixelRed(q));
                SetPixelBlue(q,GetPixelRed(q));
                p++;
                if (target_channels > 1)
                  {
                    SetPixelGreen(q,ScaleShortToQuantum(*p));
                    p++;
                    SetPixelBlue(q,ScaleShortToQuantum(*p));
                    p++;
                  }
                if (target_channels > 3)
                  {
                    SetPixelIndex(indexes+x,ScaleShortToQuantum(*p));
                    p++;
                  }
                q++;
              }
              sync=SyncCacheViewAuthenticPixels(image_view,exception);
              if (sync == MagickFalse)
                status=MagickFalse;
              if (image->progress_monitor != (MagickProgressMonitor) NULL)
                {
                  MagickBooleanType
                    proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
                  #pragma omp critical (MagickCore_ProfileImage)
#endif
                  proceed=SetImageProgress(image,ProfileImageTag,progress++,
                    image->rows);
                  if (proceed == MagickFalse)
                    status=MagickFalse;
                }
            }
            image_view=DestroyCacheView(image_view);
            (void) SetImageColorspace(image,target_colorspace);
            switch (signature)
            {
              case cmsSigRgbData:
              {
                image->type=image->matte == MagickFalse ? TrueColorType :
                  TrueColorMatteType;
                break;
              }
              case cmsSigCmykData:
              {
                image->type=image->matte == MagickFalse ? ColorSeparationType :
                  ColorSeparationMatteType;
                break;
              }
              case cmsSigGrayData:
              {
                image->type=image->matte == MagickFalse ? GrayscaleType :
                  GrayscaleMatteType;
                break;
              }
              default:
                break;
            }
            target_pixels=DestroyPixelThreadSet(target_pixels);
            source_pixels=DestroyPixelThreadSet(source_pixels);
            transform=DestroyTransformThreadSet(transform);
            if ((status != MagickFalse) ||
                (cmsGetDeviceClass(source_profile) != cmsSigLinkClass))
              status=SetImageProfile(image,name,profile);
            if (target_profile != (cmsHPROFILE) NULL)
              (void) cmsCloseProfile(target_profile);
          }
        (void) cmsCloseProfile(source_profile);
      }
#endif
    }
  profile=DestroyStringInfo(profile);
  return(status);
}
