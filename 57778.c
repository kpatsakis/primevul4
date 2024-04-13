static void WriteIndices(Image *image, const DDSVector3 start,
  const DDSVector3 end, unsigned char* indices)
{
  register ssize_t
    i;

  size_t
    a,
    b;

  unsigned char
    remapped[16];

  const unsigned char
    *ind;

  a = ColorTo565(start);
  b = ColorTo565(end);

  for (i=0; i<16; i++)
  {
    if( a < b )
      remapped[i] = (indices[i] ^ 0x1) & 0x3;
    else if( a == b )
      remapped[i] = 0;
    else
      remapped[i] = indices[i];
  }

  if( a < b )
    Swap(a,b);

  (void) WriteBlobByte(image,(unsigned char) (a & 0xff));
  (void) WriteBlobByte(image,(unsigned char) (a >> 8));
  (void) WriteBlobByte(image,(unsigned char) (b & 0xff));
  (void) WriteBlobByte(image,(unsigned char) (b >> 8));

  for (i=0; i<4; i++)
  {
     ind = remapped + 4*i;
     (void) WriteBlobByte(image,ind[0] | (ind[1] << 2) | (ind[2] << 4) |
       (ind[3] << 6));
  }
}
