void CLASS olympus_load_raw()
{
  ushort huff[4096];
  int row, col, nbits, sign, low, high, i, c, w, n, nw;
  int acarry[2][3], *carry, pred, diff;

  huff[n=0] = 0xc0c;
  for (i=12; i--; )
    FORC(2048 >> i) huff[++n] = (i+1) << 8 | i;
  fseek (ifp, 7, SEEK_CUR);
  getbits(-1);
  for (row=0; row < height; row++) {
    memset (acarry, 0, sizeof acarry);
    for (col=0; col < raw_width; col++) {
      carry = acarry[col & 1];
      i = 2 * (carry[2] < 3);
      for (nbits=2+i; (ushort) carry[0] >> (nbits+i); nbits++);
      low = (sign = getbits(3)) & 3;
      sign = sign << 29 >> 31;
      if ((high = getbithuff(12,huff)) == 12)
	high = getbits(16-nbits) >> 1;
      carry[0] = (high << nbits) | getbits(nbits);
      diff = (carry[0] ^ sign) + carry[1];
      carry[1] = (diff*3 + carry[1]) >> 5;
      carry[2] = carry[0] > 16 ? 0 : carry[2]+1;
      if (col >= width) continue;
      if (row < 2 && col < 2) pred = 0;
      else if (row < 2) pred = BAYER(row,col-2);
      else if (col < 2) pred = BAYER(row-2,col);
      else {
	w  = BAYER(row,col-2);
	n  = BAYER(row-2,col);
	nw = BAYER(row-2,col-2);
	if ((w < nw && nw < n) || (n < nw && nw < w)) {
	  if (ABS(w-nw) > 32 || ABS(n-nw) > 32)
	    pred = w + n - nw;
	  else pred = (w + n) >> 1;
	} else pred = ABS(w-nw) > ABS(n-nw) ? w : n;
      }
      if ((BAYER(row,col) = pred + ((diff << 2) | low)) >> 12) derror();
    }
  }
}
