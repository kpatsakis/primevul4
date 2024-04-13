static MngPair mng_read_pair(MngPair previous_pair,int delta_type,
  unsigned char *p)
{
  MngPair
    pair;

  /*
    Read two ssize_t's from CLON, MOVE or PAST chunk
  */
  pair.a=(long) (((png_uint_32) p[0] << 24) | ((png_uint_32) p[1] << 16) |
    ((png_uint_32) p[2] << 8) | (png_uint_32) p[3]);
  pair.b=(long) (((png_uint_32) p[4] << 24) | ((png_uint_32) p[5] << 16) |
    ((png_uint_32) p[6] << 8) | (png_uint_32) p[7]);
  if (delta_type != 0)
    {
      pair.a+=previous_pair.a;
      pair.b+=previous_pair.b;
    }

  return(pair);
}
