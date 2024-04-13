void CLASS foveon_load_raw()
{
  struct decode *dindex;
  short diff[1024];
  unsigned bitbuf=0;
  int pred[3], fixed, row, col, bit=-1, c, i;

  fixed = get4();
  read_shorts ((ushort *) diff, 1024);
  if (!fixed) foveon_decoder (1024, 0);

  for (row=0; row < height; row++) {
    memset (pred, 0, sizeof pred);
    if (!bit && !fixed && atoi(model+2) < 14) get4();
    for (col=bit=0; col < width; col++) {
      if (fixed) {
	bitbuf = get4();
	FORC3 pred[2-c] += diff[bitbuf >> c*10 & 0x3ff];
      }
      else FORC3 {
	for (dindex=first_decode; dindex->branch[0]; ) {
	  if ((bit = (bit-1) & 31) == 31)
	    for (i=0; i < 4; i++)
	      bitbuf = (bitbuf << 8) + fgetc(ifp);
	  dindex = dindex->branch[bitbuf >> bit & 1];
	}
	pred[c] += diff[dindex->leaf];
	if (pred[c] >> 16 && ~pred[c] >> 16) derror();
      }
      FORC3 image[row*width+col][c] = pred[c];
    }
  }
  if (document_mode)
    for (i=0; i < height*width*4; i++)
      if ((short) image[0][i] < 0) image[0][i] = 0;
  foveon_load_camf();
}
