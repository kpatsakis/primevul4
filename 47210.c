void CLASS adobe_dng_load_raw_nc()
{
  ushort *pixel, *rp;
  unsigned row, col;

  pixel = (ushort *) calloc (raw_width * tiff_samples, sizeof *pixel);
  merror (pixel, "adobe_dng_load_raw_nc()");
  for (row=0; row < raw_height; row++) {
    if (tiff_bps == 16)
      read_shorts (pixel, raw_width * tiff_samples);
    else {
      getbits(-1);
      for (col=0; col < raw_width * tiff_samples; col++)
	pixel[col] = getbits(tiff_bps);
    }
    for (rp=pixel, col=0; col < raw_width; col++)
      adobe_copy_pixel (row, col, &rp);
  }
  free (pixel);
}
