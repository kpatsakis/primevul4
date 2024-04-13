static void WriteResolutionResourceBlock(Image *image)
{
  double
    x_resolution,
    y_resolution;

  unsigned short
    units;

  if (image->units == PixelsPerCentimeterResolution)
    {
      x_resolution=2.54*65536.0*image->resolution.x+0.5;
      y_resolution=2.54*65536.0*image->resolution.y+0.5;
      units=2;
    }
  else
    {
      x_resolution=65536.0*image->resolution.x+0.5;
      y_resolution=65536.0*image->resolution.y+0.5;
      units=1;
    }
  (void) WriteBlob(image,4,(const unsigned char *) "8BIM");
  (void) WriteBlobMSBShort(image,0x03ED);
  (void) WriteBlobMSBShort(image,0);
  (void) WriteBlobMSBLong(image,16); /* resource size */
  (void) WriteBlobMSBLong(image,(unsigned int) (x_resolution+0.5));
  (void) WriteBlobMSBShort(image,units); /* horizontal resolution unit */
  (void) WriteBlobMSBShort(image,units); /* width unit */
  (void) WriteBlobMSBLong(image,(unsigned int) (y_resolution+0.5));
  (void) WriteBlobMSBShort(image,units); /* vertical resolution unit */
  (void) WriteBlobMSBShort(image,units); /* height unit */
}
