static inline MagickOffsetType *ReadPSDRLESizes(Image *image,
  const PSDInfo *psd_info,const size_t size)
{
  MagickOffsetType
    *sizes;

  ssize_t
    y;

  sizes=(MagickOffsetType *) AcquireQuantumMemory(size,sizeof(*sizes));
  if(sizes != (MagickOffsetType *) NULL)
    {
      for (y=0; y < (ssize_t) size; y++)
      {
        if (psd_info->version == 1)
          sizes[y]=(MagickOffsetType) ReadBlobShort(image);
        else
          sizes[y]=(MagickOffsetType) ReadBlobLong(image);
      }
    }
  return sizes;
}
