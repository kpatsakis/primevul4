void CLASS phase_one_load_raw()
{
  int row, col, a, b;
  ushort *pixel, akey, bkey, mask;

  fseek (ifp, ph1.key_off, SEEK_SET);
  akey = get2();
  bkey = get2();
  mask = ph1.format == 1 ? 0x5555:0x1354;
  fseek (ifp, data_offset + top_margin*raw_width*2, SEEK_SET);
  pixel = (ushort *) calloc (raw_width, sizeof *pixel);
  merror (pixel, "phase_one_load_raw()");
  for (row=0; row < height; row++) {
    read_shorts (pixel, raw_width);
    if (ph1.format)
      for (col=0; col < raw_width; col+=2) {
	a = pixel[col+0] ^ akey;
	b = pixel[col+1] ^ bkey;
	pixel[col+0] = (a & mask) | (b & ~mask);
	pixel[col+1] = (b & mask) | (a & ~mask);
      }
    for (col=0; col < width; col++)
      BAYER(row,col) = pixel[col+left_margin];
  }
  free (pixel);
  phase_one_correct();
}
