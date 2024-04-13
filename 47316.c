void CLASS phase_one_load_raw_c()
{
  static const int length[] = { 8,7,6,9,11,10,5,12,14,13 };
  int *offset, len[2], pred[2], row, col, i, j;
  ushort *pixel;
  short (*black)[2];

  pixel = (ushort *) calloc (raw_width + raw_height*4, 2);
  merror (pixel, "phase_one_load_raw_c()");
  offset = (int *) (pixel + raw_width);
  fseek (ifp, strip_offset, SEEK_SET);
  for (row=0; row < raw_height; row++)
    offset[row] = get4();
  black = (short (*)[2]) offset + raw_height;
  fseek (ifp, ph1.black_off, SEEK_SET);
  if (ph1.black_off)
    read_shorts ((ushort *) black[0], raw_height*2);
  for (i=0; i < 256; i++)
    curve[i] = i*i / 3.969 + 0.5;
  for (row=0; row < raw_height; row++) {
    fseek (ifp, data_offset + offset[row], SEEK_SET);
    ph1_bits(-1);
    pred[0] = pred[1] = 0;
    for (col=0; col < raw_width; col++) {
      if (col >= (raw_width & -8))
	len[0] = len[1] = 14;
      else if ((col & 7) == 0)
	for (i=0; i < 2; i++) {
	  for (j=0; j < 5 && !ph1_bits(1); j++);
	  if (j--) len[i] = length[j*2 + ph1_bits(1)];
	}
      if ((i = len[col & 1]) == 14)
	pixel[col] = pred[col & 1] = ph1_bits(16);
      else
	pixel[col] = pred[col & 1] += ph1_bits(i) + 1 - (1 << (i - 1));
      if (pred[col & 1] >> 16) derror();
      if (ph1.format == 5 && pixel[col] < 256)
	pixel[col] = curve[pixel[col]];
    }
    if ((unsigned) (row-top_margin) < height)
      for (col=0; col < width; col++) {
	i = (pixel[col+left_margin] << 2)
		- ph1.black + black[row][col >= ph1.split_col];
	if (i > 0) BAYER(row-top_margin,col) = i;
      }
  }
  free (pixel);
  phase_one_correct();
  maximum = 0xfffc - ph1.black;
}
