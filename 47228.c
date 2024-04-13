void CLASS eight_bit_load_raw()
{
  uchar *pixel;
  unsigned row, col, val, lblack=0;

  pixel = (uchar *) calloc (raw_width, sizeof *pixel);
  merror (pixel, "eight_bit_load_raw()");
  fseek (ifp, top_margin*raw_width, SEEK_CUR);
  for (row=0; row < height; row++) {
    if (fread (pixel, 1, raw_width, ifp) < raw_width) derror();
    for (col=0; col < raw_width; col++) {
      val = curve[pixel[col]];
      if ((unsigned) (col-left_margin) < width)
	BAYER(row,col-left_margin) = val;
      else lblack += val;
    }
  }
  free (pixel);
  if (raw_width > width+1)
    black = lblack / ((raw_width - width) * height);
  if (!strncmp(model,"DC2",3))
    black = 0;
  maximum = curve[0xff];
}
