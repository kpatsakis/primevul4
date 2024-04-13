static MagickBooleanType load_tile(Image *image,Image *tile_image,
  XCFDocInfo *inDocInfo,XCFLayerInfo *inLayerInfo,size_t data_length)
{
  ExceptionInfo
    *exception;

  ssize_t
    y;

  register ssize_t
    x;

  register PixelPacket
    *q;

  size_t
    extent;

  ssize_t
    count;

  unsigned char
    *graydata;

  XCFPixelPacket
    *xcfdata,
    *xcfodata;

  extent=0;
  if (inDocInfo->image_type == GIMP_GRAY)
    extent=tile_image->columns*tile_image->rows*sizeof(*graydata);
  else
    if (inDocInfo->image_type == GIMP_RGB)
      extent=tile_image->columns*tile_image->rows*sizeof(*xcfdata);
  if (extent > data_length)
    ThrowBinaryException(CorruptImageError,"NotEnoughPixelData",
      image->filename);
  xcfdata=(XCFPixelPacket *) AcquireQuantumMemory(MagickMax(data_length,
    tile_image->columns*tile_image->rows),sizeof(*xcfdata));
  if (xcfdata == (XCFPixelPacket *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  xcfodata=xcfdata;
  graydata=(unsigned char *) xcfdata;  /* used by gray and indexed */
  count=ReadBlob(image,data_length,(unsigned char *) xcfdata);
  if (count != (ssize_t) data_length)
    ThrowBinaryException(CorruptImageError,"NotEnoughPixelData",
      image->filename);
  exception=(&image->exception);
  for (y=0; y < (ssize_t) tile_image->rows; y++)
  {
    q=GetAuthenticPixels(tile_image,0,y,tile_image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    if (inDocInfo->image_type == GIMP_GRAY)
      {
        for (x=0; x < (ssize_t) tile_image->columns; x++)
        {
          SetPixelRed(q,ScaleCharToQuantum(*graydata));
          SetPixelGreen(q,GetPixelRed(q));
          SetPixelBlue(q,GetPixelRed(q));
          SetPixelAlpha(q,ScaleCharToQuantum((unsigned char)
            inLayerInfo->alpha));
          graydata++;
          q++;
        }
      }
    else
      if (inDocInfo->image_type == GIMP_RGB)
        {
          for (x=0; x < (ssize_t) tile_image->columns; x++)
          {
            SetPixelRed(q,ScaleCharToQuantum(xcfdata->red));
            SetPixelGreen(q,ScaleCharToQuantum(xcfdata->green));
            SetPixelBlue(q,ScaleCharToQuantum(xcfdata->blue));
            SetPixelAlpha(q,xcfdata->alpha == 255U ? TransparentOpacity :
              ScaleCharToQuantum((unsigned char) inLayerInfo->alpha));
            xcfdata++;
            q++;
          }
        }
     if (SyncAuthenticPixels(tile_image,exception) == MagickFalse)
       break;
  }
  xcfodata=(XCFPixelPacket *) RelinquishMagickMemory(xcfodata);
  return MagickTrue;
}
