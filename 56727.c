static long mng_get_long(unsigned char *p)
{
  return ((long) (((png_uint_32) p[0] << 24) | ((png_uint_32) p[1] << 16) |
    ((png_uint_32) p[2] << 8) | (png_uint_32) p[3]));
}
