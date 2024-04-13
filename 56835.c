static MagickBooleanType WriteOneJNGImage(MngInfo *mng_info,
   const ImageInfo *image_info,Image *image,ExceptionInfo *exception)
{
  Image
    *jpeg_image;

  ImageInfo
    *jpeg_image_info;

  MagickBooleanType
    logging,
    status;

  size_t
    length;

  unsigned char
    *blob,
    chunk[80],
    *p;

  unsigned int
    jng_alpha_compression_method,
    jng_alpha_sample_depth,
    jng_color_type,
    transparent;

  size_t
    jng_alpha_quality,
    jng_quality;

  logging=LogMagickEvent(CoderEvent,GetMagickModule(),
    "  Enter WriteOneJNGImage()");

  blob=(unsigned char *) NULL;
  jpeg_image=(Image *) NULL;
  jpeg_image_info=(ImageInfo *) NULL;
  length=0;

  status=MagickTrue;
  transparent=image_info->type==GrayscaleAlphaType ||
     image_info->type==TrueColorAlphaType ||
     image->alpha_trait != UndefinedPixelTrait;

  jng_alpha_sample_depth = 0;

  jng_quality=image_info->quality == 0UL ? 75UL : image_info->quality%1000;

  jng_alpha_compression_method=image->compression==JPEGCompression? 8 : 0;

  jng_alpha_quality=image_info->quality == 0UL ? 75UL :
      image_info->quality;

  if (jng_alpha_quality >= 1000)
    jng_alpha_quality /= 1000;

  length=0;

  if (transparent != 0)
    {
      jng_color_type=14;

      /* Create JPEG blob, image, and image_info */
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  Creating jpeg_image_info for alpha.");

      jpeg_image_info=(ImageInfo *) CloneImageInfo(image_info);

      if (jpeg_image_info == (ImageInfo *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  Creating jpeg_image.");

      jpeg_image=SeparateImage(image,AlphaChannel,exception);
      if (jpeg_image == (Image *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      (void) CopyMagickString(jpeg_image->magick,"JPEG",MagickPathExtent);
      jpeg_image->alpha_trait=UndefinedPixelTrait;
      jpeg_image->quality=jng_alpha_quality;
      jpeg_image_info->type=GrayscaleType;
      (void) SetImageType(jpeg_image,GrayscaleType,exception);
      (void) AcquireUniqueFilename(jpeg_image->filename);
      (void) FormatLocaleString(jpeg_image_info->filename,MagickPathExtent,
        "%s",jpeg_image->filename);
    }
  else
    {
      jng_alpha_compression_method=0;
      jng_color_type=10;
      jng_alpha_sample_depth=0;
    }

  /* To do: check bit depth of PNG alpha channel */

  /* Check if image is grayscale. */
  if (image_info->type != TrueColorAlphaType && image_info->type !=
    TrueColorType && SetImageGray(image,exception))
    jng_color_type-=2;

  if (logging != MagickFalse)
    {
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    JNG Quality           = %d",(int) jng_quality);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    JNG Color Type        = %d",jng_color_type);
        if (transparent != 0)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    JNG Alpha Compression = %d",jng_alpha_compression_method);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    JNG Alpha Depth       = %d",jng_alpha_sample_depth);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    JNG Alpha Quality     = %d",(int) jng_alpha_quality);
          }
    }

  if (transparent != 0)
    {
      if (jng_alpha_compression_method==0)
        {
          const char
            *value;

          /* Encode alpha as a grayscale PNG blob */
          status=OpenBlob(jpeg_image_info,jpeg_image,WriteBinaryBlobMode,
            exception);
          if (status == MagickFalse)
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");

          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Creating PNG blob.");

          (void) CopyMagickString(jpeg_image_info->magick,"PNG",
             MagickPathExtent);
          (void) CopyMagickString(jpeg_image->magick,"PNG",MagickPathExtent);
          jpeg_image_info->interlace=NoInterlace;

          /* Exclude all ancillary chunks */
          (void) SetImageArtifact(jpeg_image,"png:exclude-chunks","all");

          blob=(unsigned char *) ImageToBlob(jpeg_image_info,jpeg_image,
            &length,exception);

          /* Retrieve sample depth used */
          value=GetImageProperty(jpeg_image,"png:bit-depth-written",exception);
          if (value != (char *) NULL)
            jng_alpha_sample_depth= (unsigned int) value[0];
        }
      else
        {
          /* Encode alpha as a grayscale JPEG blob */

          status=OpenBlob(jpeg_image_info,jpeg_image,WriteBinaryBlobMode,
            exception);
          if (status == MagickFalse)
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");

          (void) CopyMagickString(jpeg_image_info->magick,"JPEG",
            MagickPathExtent);
          (void) CopyMagickString(jpeg_image->magick,"JPEG",MagickPathExtent);
          jpeg_image_info->interlace=NoInterlace;
          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Creating blob.");
          blob=(unsigned char *) ImageToBlob(jpeg_image_info,
             jpeg_image,&length,
           exception);
          jng_alpha_sample_depth=8;

          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Successfully read jpeg_image into a blob, length=%.20g.",
              (double) length);

        }
      /* Destroy JPEG image and image_info */
      jpeg_image=DestroyImage(jpeg_image);
      (void) RelinquishUniqueFileResource(jpeg_image_info->filename);
      jpeg_image_info=DestroyImageInfo(jpeg_image_info);
    }

  /* Write JHDR chunk */
  (void) WriteBlobMSBULong(image,16L);  /* chunk data length=16 */
  PNGType(chunk,mng_JHDR);
  LogPNGChunk(logging,mng_JHDR,16L);
  PNGLong(chunk+4,(png_uint_32) image->columns);
  PNGLong(chunk+8,(png_uint_32) image->rows);
  chunk[12]=jng_color_type;
  chunk[13]=8;  /* sample depth */
  chunk[14]=8; /*jng_image_compression_method */
  chunk[15]=(unsigned char) (image_info->interlace == NoInterlace ? 0 : 8);
  chunk[16]=jng_alpha_sample_depth;
  chunk[17]=jng_alpha_compression_method;
  chunk[18]=0; /*jng_alpha_filter_method */
  chunk[19]=0; /*jng_alpha_interlace_method */
  (void) WriteBlob(image,20,chunk);
  (void) WriteBlobMSBULong(image,crc32(0,chunk,20));
  if (logging != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG width:%15lu",(unsigned long) image->columns);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG height:%14lu",(unsigned long) image->rows);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG color type:%10d",jng_color_type);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG sample depth:%8d",8);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG compression:%9d",8);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG interlace:%11d",0);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG alpha depth:%9d",jng_alpha_sample_depth);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG alpha compression:%3d",jng_alpha_compression_method);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG alpha filter:%8d",0);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    JNG alpha interlace:%5d",0);
    }

  /* Write any JNG-chunk-b profiles */
  (void) Magick_png_write_chunk_from_profile(image,"JNG-chunk-b",logging);

  /*
     Write leading ancillary chunks
  */

  if (transparent != 0)
  {
    /*
      Write JNG bKGD chunk
    */

    unsigned char
      blue,
      green,
      red;

    ssize_t
      num_bytes;

    if (jng_color_type == 8 || jng_color_type == 12)
      num_bytes=6L;
    else
      num_bytes=10L;
    (void) WriteBlobMSBULong(image,(size_t) (num_bytes-4L));
    PNGType(chunk,mng_bKGD);
    LogPNGChunk(logging,mng_bKGD,(size_t) (num_bytes-4L));
    red=ScaleQuantumToChar(image->background_color.red);
    green=ScaleQuantumToChar(image->background_color.green);
    blue=ScaleQuantumToChar(image->background_color.blue);
    *(chunk+4)=0;
    *(chunk+5)=red;
    *(chunk+6)=0;
    *(chunk+7)=green;
    *(chunk+8)=0;
    *(chunk+9)=blue;
    (void) WriteBlob(image,(size_t) num_bytes,chunk);
    (void) WriteBlobMSBULong(image,crc32(0,chunk,(uInt) num_bytes));
  }

  if ((image->colorspace == sRGBColorspace || image->rendering_intent))
    {
      /*
        Write JNG sRGB chunk
      */
      (void) WriteBlobMSBULong(image,1L);
      PNGType(chunk,mng_sRGB);
      LogPNGChunk(logging,mng_sRGB,1L);

      if (image->rendering_intent != UndefinedIntent)
        chunk[4]=(unsigned char)
          Magick_RenderingIntent_to_PNG_RenderingIntent(
          (image->rendering_intent));

      else
        chunk[4]=(unsigned char)
          Magick_RenderingIntent_to_PNG_RenderingIntent(
          (PerceptualIntent));

      (void) WriteBlob(image,5,chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,5));
    }
  else
    {
      if (image->gamma != 0.0)
        {
          /*
             Write JNG gAMA chunk
          */
          (void) WriteBlobMSBULong(image,4L);
          PNGType(chunk,mng_gAMA);
          LogPNGChunk(logging,mng_gAMA,4L);
          PNGLong(chunk+4,(png_uint_32) (100000*image->gamma+0.5));
          (void) WriteBlob(image,8,chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,8));
        }

      if ((mng_info->equal_chrms == MagickFalse) &&
          (image->chromaticity.red_primary.x != 0.0))
        {
          PrimaryInfo
            primary;

          /*
             Write JNG cHRM chunk
          */
          (void) WriteBlobMSBULong(image,32L);
          PNGType(chunk,mng_cHRM);
          LogPNGChunk(logging,mng_cHRM,32L);
          primary=image->chromaticity.white_point;
          PNGLong(chunk+4,(png_uint_32) (100000*primary.x+0.5));
          PNGLong(chunk+8,(png_uint_32) (100000*primary.y+0.5));
          primary=image->chromaticity.red_primary;
          PNGLong(chunk+12,(png_uint_32) (100000*primary.x+0.5));
          PNGLong(chunk+16,(png_uint_32) (100000*primary.y+0.5));
          primary=image->chromaticity.green_primary;
          PNGLong(chunk+20,(png_uint_32) (100000*primary.x+0.5));
          PNGLong(chunk+24,(png_uint_32) (100000*primary.y+0.5));
          primary=image->chromaticity.blue_primary;
          PNGLong(chunk+28,(png_uint_32) (100000*primary.x+0.5));
          PNGLong(chunk+32,(png_uint_32) (100000*primary.y+0.5));
          (void) WriteBlob(image,36,chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,36));
        }
    }

  if (image->resolution.x && image->resolution.y && !mng_info->equal_physs)
    {
      /*
         Write JNG pHYs chunk
      */
      (void) WriteBlobMSBULong(image,9L);
      PNGType(chunk,mng_pHYs);
      LogPNGChunk(logging,mng_pHYs,9L);
      if (image->units == PixelsPerInchResolution)
        {
          PNGLong(chunk+4,(png_uint_32)
            (image->resolution.x*100.0/2.54+0.5));

          PNGLong(chunk+8,(png_uint_32)
            (image->resolution.y*100.0/2.54+0.5));

          chunk[12]=1;
        }

      else
        {
          if (image->units == PixelsPerCentimeterResolution)
            {
              PNGLong(chunk+4,(png_uint_32)
                (image->resolution.x*100.0+0.5));

              PNGLong(chunk+8,(png_uint_32)
                (image->resolution.y*100.0+0.5));

              chunk[12]=1;
            }

          else
            {
              PNGLong(chunk+4,(png_uint_32) (image->resolution.x+0.5));
              PNGLong(chunk+8,(png_uint_32) (image->resolution.y+0.5));
              chunk[12]=0;
            }
        }
      (void) WriteBlob(image,13,chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,13));
    }

  if (mng_info->write_mng == 0 && (image->page.x || image->page.y))
    {
      /*
         Write JNG oFFs chunk
      */
      (void) WriteBlobMSBULong(image,9L);
      PNGType(chunk,mng_oFFs);
      LogPNGChunk(logging,mng_oFFs,9L);
      PNGsLong(chunk+4,(ssize_t) (image->page.x));
      PNGsLong(chunk+8,(ssize_t) (image->page.y));
      chunk[12]=0;
      (void) WriteBlob(image,13,chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,13));
    }
  if (mng_info->write_mng == 0 && (image->page.width || image->page.height))
    {
       (void) WriteBlobMSBULong(image,9L);  /* data length=8 */
       PNGType(chunk,mng_vpAg);
       LogPNGChunk(logging,mng_vpAg,9L);
       PNGLong(chunk+4,(png_uint_32) image->page.width);
       PNGLong(chunk+8,(png_uint_32) image->page.height);
       chunk[12]=0;   /* unit = pixels */
       (void) WriteBlob(image,13,chunk);
       (void) WriteBlobMSBULong(image,crc32(0,chunk,13));
    }

  if (transparent != 0)
    {
      if (jng_alpha_compression_method==0)
        {
          register ssize_t
            i;

          size_t
            len;

          /* Write IDAT chunk header */
          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Write IDAT chunks from blob, length=%.20g.",(double)
              length);

          /* Copy IDAT chunks */
          len=0;
          p=blob+8;
          for (i=8; i<(ssize_t) length; i+=len+12)
          {
            len=(size_t) (*p) << 24;
            len|=(size_t) (*(p+1)) << 16;
            len|=(size_t) (*(p+2)) << 8;
            len|=(size_t) (*(p+3));
            p+=4;

            if (*(p)==73 && *(p+1)==68 && *(p+2)==65 && *(p+3)==84) /* IDAT */
              {
                /* Found an IDAT chunk. */
                (void) WriteBlobMSBULong(image,len);
                LogPNGChunk(logging,mng_IDAT,len);
                (void) WriteBlob(image,len+4,p);
                (void) WriteBlobMSBULong(image, crc32(0,p,(uInt) len+4));
              }

            else
              {
                if (logging != MagickFalse)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "    Skipping %c%c%c%c chunk, length=%.20g.",
                    *(p),*(p+1),*(p+2),*(p+3),(double) len);
              }
            p+=(8+len);
          }
        }
      else if (length != 0)
        {
          /* Write JDAA chunk header */
          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Write JDAA chunk, length=%.20g.",(double) length);
          (void) WriteBlobMSBULong(image,(size_t) length);
          PNGType(chunk,mng_JDAA);
          LogPNGChunk(logging,mng_JDAA,length);
          /* Write JDAT chunk(s) data */
          (void) WriteBlob(image,4,chunk);
          (void) WriteBlob(image,length,blob);
          (void) WriteBlobMSBULong(image,crc32(crc32(0,chunk,4),blob,
             (uInt) length));
        }
      blob=(unsigned char *) RelinquishMagickMemory(blob);
    }

  /* Encode image as a JPEG blob */
  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Creating jpeg_image_info.");
  jpeg_image_info=(ImageInfo *) CloneImageInfo(image_info);
  if (jpeg_image_info == (ImageInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Creating jpeg_image.");

  jpeg_image=CloneImage(image,0,0,MagickTrue,exception);
  if (jpeg_image == (Image *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  (void) CopyMagickString(jpeg_image->magick,"JPEG",MagickPathExtent);

  (void) AcquireUniqueFilename(jpeg_image->filename);
  (void) FormatLocaleString(jpeg_image_info->filename,MagickPathExtent,"%s",
    jpeg_image->filename);

  status=OpenBlob(jpeg_image_info,jpeg_image,WriteBinaryBlobMode,
    exception);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Created jpeg_image, %.20g x %.20g.",(double) jpeg_image->columns,
      (double) jpeg_image->rows);

  if (status == MagickFalse)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");

  if (jng_color_type == 8 || jng_color_type == 12)
    jpeg_image_info->type=GrayscaleType;

  jpeg_image_info->quality=jng_quality;
  jpeg_image->quality=jng_quality;
  (void) CopyMagickString(jpeg_image_info->magick,"JPEG",MagickPathExtent);
  (void) CopyMagickString(jpeg_image->magick,"JPEG",MagickPathExtent);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Creating blob.");

  blob=(unsigned char *) ImageToBlob(jpeg_image_info,jpeg_image,&length,
    exception);

  if (logging != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Successfully read jpeg_image into a blob, length=%.20g.",
        (double) length);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Write JDAT chunk, length=%.20g.",(double) length);
    }

  /* Write JDAT chunk(s) */
  (void) WriteBlobMSBULong(image,(size_t) length);
  PNGType(chunk,mng_JDAT);
  LogPNGChunk(logging,mng_JDAT,length);
  (void) WriteBlob(image,4,chunk);
  (void) WriteBlob(image,length,blob);
  (void) WriteBlobMSBULong(image,crc32(crc32(0,chunk,4),blob,(uInt) length));

  jpeg_image=DestroyImage(jpeg_image);
  (void) RelinquishUniqueFileResource(jpeg_image_info->filename);
  jpeg_image_info=DestroyImageInfo(jpeg_image_info);
  blob=(unsigned char *) RelinquishMagickMemory(blob);

  /* Write any JNG-chunk-e profiles */
  (void) Magick_png_write_chunk_from_profile(image,"JNG-chunk-e",logging);

  /* Write IEND chunk */
  (void) WriteBlobMSBULong(image,0L);
  PNGType(chunk,mng_IEND);
  LogPNGChunk(logging,mng_IEND,0);
  (void) WriteBlob(image,4,chunk);
  (void) WriteBlobMSBULong(image,crc32(0,chunk,4));

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  exit WriteOneJNGImage()");

  return(status);
}
