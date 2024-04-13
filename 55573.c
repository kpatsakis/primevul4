static inline ssize_t SetPSDOffset(const PSDInfo *psd_info,Image *image,
  const size_t offset)
{
  if (psd_info->version == 1)
    return(WriteBlobMSBShort(image,(unsigned short) offset));
  return(WriteBlobMSBLong(image,(unsigned short) offset));
}
