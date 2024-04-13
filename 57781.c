static void WriteUncompressed(Image *image, ExceptionInfo *exception)
{
  register const PixelPacket
    *p;

  register ssize_t
    x;

  ssize_t
    y;

  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;

    for (x=0; x < (ssize_t) image->columns; x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(GetPixelBlue(p)));
      (void) WriteBlobByte(image,ScaleQuantumToChar(GetPixelGreen(p)));
      (void) WriteBlobByte(image,ScaleQuantumToChar(GetPixelRed(p)));
      if (image->matte)
        (void) WriteBlobByte(image,ScaleQuantumToChar(GetPixelAlpha(p)));
      p++;
    }
  }
}
