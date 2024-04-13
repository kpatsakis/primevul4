static size_t WriteCompressionStart(const PSDInfo *psd_info,Image *image,
  const Image *next_image,const ssize_t channels)
{
  size_t
    length;

  ssize_t
    i,
    y;

  if (next_image->compression == RLECompression)
    {
      length=WriteBlobMSBShort(image,RLE);
      for (i=0; i < channels; i++)
        for (y=0; y < (ssize_t) next_image->rows; y++)
          length+=SetPSDOffset(psd_info,image,0);
    }
#ifdef MAGICKCORE_ZLIB_DELEGATE
  else if (next_image->compression == ZipCompression)
    length=WriteBlobMSBShort(image,ZipWithoutPrediction);
#endif
  else
    length=WriteBlobMSBShort(image,Raw);
  return(length);
}
