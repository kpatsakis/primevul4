static MngBox mng_read_box(MngBox previous_box,char delta_type,
  unsigned char *p)
{
   MngBox
      box;

  /*
    Read clipping boundaries from DEFI, CLIP, FRAM, or PAST chunk.
  */
  box.left=(long) (((png_uint_32) p[0] << 24) | ((png_uint_32) p[1] << 16) |
    ((png_uint_32) p[2] << 8) | (png_uint_32) p[3]);
  box.right=(long) (((png_uint_32) p[4]  << 24) | ((png_uint_32) p[5] << 16) |
    ((png_uint_32) p[6] << 8) | (png_uint_32) p[7]);
  box.top=(long) (((png_uint_32) p[8]  << 24) | ((png_uint_32) p[9] << 16) |
    ((png_uint_32) p[10] << 8) | (png_uint_32) p[11]);
  box.bottom=(long) (((png_uint_32) p[12] << 24) | ((png_uint_32) p[13] << 16) |
    ((png_uint_32) p[14] << 8) | (png_uint_32) p[15]);
  if (delta_type != 0)
    {
      box.left+=previous_box.left;
      box.right+=previous_box.right;
      box.top+=previous_box.top;
      box.bottom+=previous_box.bottom;
    }

  return(box);
}
