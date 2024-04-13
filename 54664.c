static void Rd_WP_DWORD(Image *image,size_t *d)
{
  unsigned char
    b;

  b=ReadBlobByte(image);
  *d=b;
  if (b < 0xFFU)
    return;
  b=ReadBlobByte(image);
  *d=(size_t) b;
  b=ReadBlobByte(image);
  *d+=(size_t) b*256l;
  if (*d < 0x8000)
    return;
  *d=(*d & 0x7FFF) << 16;
  b=ReadBlobByte(image);
  *d+=(size_t) b;
  b=ReadBlobByte(image);
  *d+=(size_t) b*256l;
  return;
}
