void CLASS sony_arw2_load_raw()
{
  uchar *data, *dp;
  ushort pix[16];
  int row, col, val, max, min, imax, imin, sh, bit, i;

  data = (uchar *) malloc (raw_width);
  merror (data, "sony_arw2_load_raw()");
  for (row=0; row < height; row++) {
    fread (data, 1, raw_width, ifp);
    for (dp=data, col=0; col < raw_width-30; dp+=16) {
      max = 0x7ff & (val = sget4(dp));
      min = 0x7ff & val >> 11;
      imax = 0x0f & val >> 22;
      imin = 0x0f & val >> 26;
      for (sh=0; sh < 4 && 0x80 << sh <= max-min; sh++);
      for (bit=30, i=0; i < 16; i++)
	if      (i == imax) pix[i] = max;
	else if (i == imin) pix[i] = min;
	else {
	  pix[i] = ((sget2(dp+(bit >> 3)) >> (bit & 7) & 0x7f) << sh) + min;
	  if (pix[i] > 0x7ff) pix[i] = 0x7ff;
	  bit += 7;
	}
      for (i=0; i < 16; i++, col+=2)
	if (col < width) BAYER(row,col) = curve[pix[i] << 1] >> 2;
      col -= col & 1 ? 1:31;
    }
  }
  free (data);
}
