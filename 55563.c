static MagickBooleanType ReadPSDChannelPixels(Image *image,const size_t channels,
  const size_t row,const ssize_t type,const unsigned char *pixels,
  ExceptionInfo *exception)
{
  Quantum
    pixel;

  register const unsigned char
    *p;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  register ssize_t
    x;

  size_t
    packet_size;

  unsigned short
    nibble;

  p=pixels;
  q=GetAuthenticPixels(image,0,row,image->columns,1,exception);
  if (q == (PixelPacket *) NULL)
    return MagickFalse;
  indexes=GetAuthenticIndexQueue(image);
  packet_size=GetPSDPacketSize(image);
  for (x=0; x < (ssize_t) image->columns; x++)
  {
    if (packet_size == 1)
      pixel=ScaleCharToQuantum(*p++);
    else
      {
        p=PushShortPixel(MSBEndian,p,&nibble);
        pixel=ScaleShortToQuantum(nibble);
      }
    if (image->depth > 1)
      {
        SetPSDPixel(image,channels,type,packet_size,pixel,q++,indexes,x);
      }
    else
      {
        ssize_t
          bit,
          number_bits;

        number_bits=image->columns-x;
        if (number_bits > 8)
          number_bits=8;
        for (bit=0; bit < number_bits; bit++)
        {
          SetPSDPixel(image,channels,type,packet_size,(((unsigned char) pixel)
            & (0x01 << (7-bit))) != 0 ? 0 : QuantumRange,q++,indexes,x++);
        }
        if (x != (ssize_t) image->columns)
          x--;
        continue;
      }
  }
  return(SyncAuthenticPixels(image,exception));
}
