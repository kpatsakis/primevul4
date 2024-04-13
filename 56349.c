static MagickBooleanType PCXWritePixels(PCXInfo *pcx_info,
  const unsigned char *pixels,Image *image)
{
  register const unsigned char
    *q;

  register ssize_t
    i,
    x;

  ssize_t
    count;

  unsigned char
    packet,
    previous;

  q=pixels;
  for (i=0; i < (ssize_t) pcx_info->planes; i++)
  {
    if (pcx_info->encoding == 0)
      {
        for (x=0; x < (ssize_t) pcx_info->bytes_per_line; x++)
          (void) WriteBlobByte(image,(unsigned char) (*q++));
      }
    else
      {
        previous=(*q++);
        count=1;
        for (x=0; x < (ssize_t) (pcx_info->bytes_per_line-1); x++)
        {
          packet=(*q++);
          if ((packet == previous) && (count < 63))
            {
              count++;
              continue;
            }
          if ((count > 1) || ((previous & 0xc0) == 0xc0))
            {
              count|=0xc0;
              (void) WriteBlobByte(image,(unsigned char) count);
            }
          (void) WriteBlobByte(image,previous);
          previous=packet;
          count=1;
        }
        if ((count > 1) || ((previous & 0xc0) == 0xc0))
          {
            count|=0xc0;
            (void) WriteBlobByte(image,(unsigned char) count);
          }
        (void) WriteBlobByte(image,previous);
      }
  }
  return (MagickTrue);
}
