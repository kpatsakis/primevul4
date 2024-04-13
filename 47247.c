void CLASS fuji_load_raw()
{
  ushort *pixel;
  int wide, row, col, r, c;

  fseek (ifp, (top_margin*raw_width + left_margin) * 2, SEEK_CUR);
  wide = fuji_width << !fuji_layout;
  pixel = (ushort *) calloc (wide, sizeof *pixel);
  merror (pixel, "fuji_load_raw()");
  for (row=0; row < raw_height; row++) {
    read_shorts (pixel, wide);
    fseek (ifp, 2*(raw_width - wide), SEEK_CUR);
    for (col=0; col < wide; col++) {
      if (fuji_layout) {
	r = fuji_width - 1 - col + (row >> 1);
	c = col + ((row+1) >> 1);
      } else {
	r = fuji_width - 1 + row - (col >> 1);
	c = row + ((col+1) >> 1);
      }
      BAYER(r,c) = pixel[col];
    }
  }
  free (pixel);
}
