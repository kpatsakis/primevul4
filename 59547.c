static int32 TIFFReadPixels(TIFF *tiff,size_t bits_per_sample,
  tsample_t sample,ssize_t row,tdata_t scanline)
{
  int32
    status;

  (void) bits_per_sample;
  status=TIFFReadScanline(tiff,scanline,(uint32) row,sample);
  return(status);
}
