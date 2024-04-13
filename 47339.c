void CLASS unpacked_load_raw()
{
  ushort *pixel;
  int row, col, bits=0;

  while (1 << ++bits < (int) maximum);
  fseek (ifp, (top_margin*raw_width + left_margin) * 2, SEEK_CUR);
  pixel = (ushort *) calloc (width, sizeof *pixel);
  merror (pixel, "unpacked_load_raw()");
  for (row=0; row < height; row++) {
    read_shorts (pixel, width);
    fseek (ifp, 2*(raw_width - width), SEEK_CUR);
    for (col=0; col < width; col++)
      if ((BAYER2(row,col) = pixel[col] >> load_flags) >> bits) derror();
  }
  free (pixel);
}
