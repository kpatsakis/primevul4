static MagickBooleanType GetTIFFInfo(const ImageInfo *image_info,TIFF *tiff,
  TIFFInfo *tiff_info)
{
  const char
    *option;

  MagickStatusType
    flags;

  uint32
    tile_columns,
    tile_rows;

  assert(tiff_info != (TIFFInfo *) NULL);
  (void) ResetMagickMemory(tiff_info,0,sizeof(*tiff_info));
  option=GetImageOption(image_info,"tiff:tile-geometry");
  if (option == (const char *) NULL)
    {
      uint32
        rows_per_strip;

      option=GetImageOption(image_info,"tiff:rows-per-strip");
      if (option != (const char *) NULL)
        rows_per_strip=(size_t) strtol(option,(char **) NULL,10);
      else
        if (TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&rows_per_strip) == 0)
          rows_per_strip=0;  /* use default */
      rows_per_strip=TIFFDefaultStripSize(tiff,rows_per_strip);
      (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,rows_per_strip);
      return(MagickTrue);
    }
  flags=ParseAbsoluteGeometry(option,&tiff_info->tile_geometry);
  if ((flags & HeightValue) == 0)
    tiff_info->tile_geometry.height=tiff_info->tile_geometry.width;
  tile_columns=(uint32) tiff_info->tile_geometry.width;
  tile_rows=(uint32) tiff_info->tile_geometry.height;
  TIFFDefaultTileSize(tiff,&tile_columns,&tile_rows);
  (void) TIFFSetField(tiff,TIFFTAG_TILEWIDTH,tile_columns);
  (void) TIFFSetField(tiff,TIFFTAG_TILELENGTH,tile_rows);
  tiff_info->tile_geometry.width=tile_columns;
  tiff_info->tile_geometry.height=tile_rows;
  tiff_info->scanlines=(unsigned char *) AcquireQuantumMemory((size_t)
    tile_rows*TIFFScanlineSize(tiff),sizeof(*tiff_info->scanlines));
  tiff_info->pixels=(unsigned char *) AcquireQuantumMemory((size_t)
    tile_rows*TIFFTileSize(tiff),sizeof(*tiff_info->scanlines));
  if ((tiff_info->scanlines == (unsigned char *) NULL) ||
      (tiff_info->pixels == (unsigned char *) NULL))
    {
      DestroyTIFFInfo(tiff_info);
      return(MagickFalse);
    }
  return(MagickTrue);
}
