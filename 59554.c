static int32 TIFFWritePixels(TIFF *tiff,TIFFInfo *tiff_info,ssize_t row,
  tsample_t sample,Image *image)
{
  int32
    status;

  register ssize_t
    i;

  register unsigned char
    *p,
    *q;

  size_t
    number_tiles,
    tile_width;

  ssize_t
    bytes_per_pixel,
    j,
    k,
    l;

  if (TIFFIsTiled(tiff) == 0)
    return(TIFFWriteScanline(tiff,tiff_info->scanline,(uint32) row,sample));
  /*
    Fill scanlines to tile height.
  */
  i=(ssize_t) (row % tiff_info->tile_geometry.height)*TIFFScanlineSize(tiff);
  (void) CopyMagickMemory(tiff_info->scanlines+i,(char *) tiff_info->scanline,
    (size_t) TIFFScanlineSize(tiff));
  if (((size_t) (row % tiff_info->tile_geometry.height) !=
      (tiff_info->tile_geometry.height-1)) &&
      (row != (ssize_t) (image->rows-1)))
    return(0);
  /*
    Write tile to TIFF image.
  */
  status=0;
  bytes_per_pixel=TIFFTileSize(tiff)/(ssize_t) (tiff_info->tile_geometry.height*
    tiff_info->tile_geometry.width);
  number_tiles=(image->columns+tiff_info->tile_geometry.width)/
    tiff_info->tile_geometry.width;
  for (i=0; i < (ssize_t) number_tiles; i++)
  {
    tile_width=(i == (ssize_t) (number_tiles-1)) ? image->columns-(i*
      tiff_info->tile_geometry.width) : tiff_info->tile_geometry.width;
    for (j=0; j < (ssize_t) ((row % tiff_info->tile_geometry.height)+1); j++)
      for (k=0; k < (ssize_t) tile_width; k++)
      {
        if (bytes_per_pixel == 0)
          {
            p=tiff_info->scanlines+(j*TIFFScanlineSize(tiff)+(i*
              tiff_info->tile_geometry.width+k)/8);
            q=tiff_info->pixels+(j*TIFFTileRowSize(tiff)+k/8);
            *q++=(*p++);
            continue;
          }
        p=tiff_info->scanlines+(j*TIFFScanlineSize(tiff)+(i*
          tiff_info->tile_geometry.width+k)*bytes_per_pixel);
        q=tiff_info->pixels+(j*TIFFTileRowSize(tiff)+k*bytes_per_pixel);
        for (l=0; l < bytes_per_pixel; l++)
          *q++=(*p++);
      }
    if ((i*tiff_info->tile_geometry.width) != image->columns)
      status=TIFFWriteTile(tiff,tiff_info->pixels,(uint32) (i*
        tiff_info->tile_geometry.width),(uint32) ((row/
        tiff_info->tile_geometry.height)*tiff_info->tile_geometry.height),0,
        sample);
    if (status < 0)
      break;
  }
  return(status);
}
