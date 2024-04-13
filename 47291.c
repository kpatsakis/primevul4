void CLASS packed_load_raw()
{
  int vbits=0, bwide, pwide, rbits, bite, half, irow, row, col, val, i;
  int zero=0;
  UINT64 bitbuf=0;

  if ((ushort)(raw_width * 8) >= width * tiff_bps) /* Is raw_width in bytes? */
       pwide = (bwide = raw_width) * 8 / tiff_bps;
  else bwide = (pwide = raw_width) * tiff_bps / 8;
  rbits = bwide * 8 - pwide * tiff_bps;
  if (load_flags & 1) bwide = bwide * 16 / 15;
  fseek (ifp, top_margin*bwide, SEEK_CUR);
  bite = 8 + (load_flags & 24);
  half = (height+1) >> 1;
  for (irow=0; irow < height; irow++) {
    row = irow;
    if (load_flags & 2 &&
	(row = irow % half * 2 + irow / half) == 1 &&
	load_flags & 4) {
      if (vbits=0, tiff_compress)
	fseek (ifp, data_offset - (-half*bwide & -2048), SEEK_SET);
      else {
	fseek (ifp, 0, SEEK_END);
	fseek (ifp, ftell(ifp) >> 3 << 2, SEEK_SET);
      }
    }
    for (col=0; col < pwide; col++) {
      for (vbits -= tiff_bps; vbits < 0; vbits += bite) {
	bitbuf <<= bite;
	for (i=0; i < bite; i+=8)
	  bitbuf |= (unsigned) (fgetc(ifp) << i);
      }
      val = bitbuf << (64-tiff_bps-vbits) >> (64-tiff_bps);
      i = (col ^ (load_flags >> 6)) - left_margin;
      if ((unsigned) i < width)
	BAYER(row,i) = val;
      else if (load_flags & 32) {
	black += val;
	zero += !val;
      }
      if (load_flags & 1 && (col % 10) == 9 &&
	fgetc(ifp) && col < width+left_margin) derror();
    }
    vbits -= rbits;
  }
  if (load_flags & 32 && pwide > width)
    black /= (pwide - width) * height;
  if (zero*4 > (pwide - width) * height)
    black = 0;
}
