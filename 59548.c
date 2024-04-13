static toff_t TIFFSeekBlob(thandle_t image,toff_t offset,int whence)
{
  return((toff_t) SeekBlob((Image *) image,(MagickOffsetType) offset,whence));
}
