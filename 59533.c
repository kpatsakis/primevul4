static TIFFMethodType GetJPEGMethod(Image* image,TIFF *tiff,uint16 photometric,
  uint16 bits_per_sample,uint16 samples_per_pixel)
{
#define BUFFER_SIZE 2048

  MagickOffsetType
    position,
    offset;

  register size_t
    i;

  TIFFMethodType
    method;

#if defined(TIFF_VERSION_BIG)
  uint64
#else
  uint32
#endif
    **value;

  unsigned char
    buffer[BUFFER_SIZE+32];

  unsigned short
    length;

  /* only support 8 bit for now */
  if ((photometric != PHOTOMETRIC_SEPARATED) || (bits_per_sample != 8) ||
      (samples_per_pixel != 4))
    return(ReadGenericMethod);
  /* Search for Adobe APP14 JPEG Marker */
  if (!TIFFGetField(tiff,TIFFTAG_STRIPOFFSETS,&value))
    return(ReadRGBAMethod);
  position=TellBlob(image);
  offset=(MagickOffsetType) (value[0]);
  if (SeekBlob(image,offset,SEEK_SET) != offset)
    return(ReadRGBAMethod);
  method=ReadRGBAMethod;
  if (ReadBlob(image,BUFFER_SIZE,buffer) == BUFFER_SIZE)
    {
      for (i=0; i < BUFFER_SIZE; i++)
      {
        while (i < BUFFER_SIZE)
        {
          if (buffer[i++] == 255)
           break;
        }
        while (i < BUFFER_SIZE)
        {
          if (buffer[++i] != 255)
           break;
        }
        if (buffer[i++] == 216) /* JPEG_MARKER_SOI */
          continue;
        length=(unsigned short) (((unsigned int) (buffer[i] << 8) |
          (unsigned int) buffer[i+1]) & 0xffff);
        if (i+(size_t) length >= BUFFER_SIZE)
          break;
        if (buffer[i-1] == 238) /* JPEG_MARKER_APP0+14 */
          {
            if (length != 14)
              break;
            /* 0 == CMYK, 1 == YCbCr, 2 = YCCK */
            if (buffer[i+13] == 2)
              method=ReadYCCKMethod;
            break;
          }
        i+=(size_t) length;
      }
    }
  (void) SeekBlob(image,position,SEEK_SET);
  return(method);
}
