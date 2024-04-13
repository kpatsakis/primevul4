void CLASS canon_sraw_load_raw()
{
  struct jhead jh;
  short *rp=0, (*ip)[4];
  int jwide, slice, scol, ecol, row, col, jrow=0, jcol=0, pix[3], c;
  int v[3]={0,0,0}, ver, hue;
  char *cp;

  if (!ljpeg_start (&jh, 0)) return;
  jwide = (jh.wide >>= 1) * jh.clrs;

  for (ecol=slice=0; slice <= cr2_slice[0]; slice++) {
    scol = ecol;
    ecol += cr2_slice[1] * 2 / jh.clrs;
    if (!cr2_slice[0] || ecol > raw_width-1) ecol = raw_width & -2;
    for (row=0; row < height; row += (jh.clrs >> 1) - 1) {
      ip = (short (*)[4]) image + row*width;
      for (col=scol; col < ecol; col+=2, jcol+=jh.clrs) {
	if ((jcol %= jwide) == 0)
	  rp = (short *) ljpeg_row (jrow++, &jh);
	if (col >= width) continue;
	FORC (jh.clrs-2)
	  ip[col + (c >> 1)*width + (c & 1)][0] = rp[jcol+c];
	ip[col][1] = rp[jcol+jh.clrs-2] - 16384;
	ip[col][2] = rp[jcol+jh.clrs-1] - 16384;
      }
    }
  }
  for (cp=model2; *cp && !isdigit(*cp); cp++);
  sscanf (cp, "%d.%d.%d", v, v+1, v+2);
  ver = (v[0]*1000 + v[1])*1000 + v[2];
  hue = (jh.sraw+1) << 2;
  if (unique_id >= 0x80000281 || (unique_id == 0x80000218 && ver > 1000006))
    hue = jh.sraw << 1;
  ip = (short (*)[4]) image;
  rp = ip[0];
  for (row=0; row < height; row++, ip+=width) {
    if (row & (jh.sraw >> 1))
      for (col=0; col < width; col+=2)
	for (c=1; c < 3; c++)
	  if (row == height-1)
	       ip[col][c] =  ip[col-width][c];
	  else ip[col][c] = (ip[col-width][c] + ip[col+width][c] + 1) >> 1;
    for (col=1; col < width; col+=2)
      for (c=1; c < 3; c++)
	if (col == width-1)
	     ip[col][c] =  ip[col-1][c];
	else ip[col][c] = (ip[col-1][c] + ip[col+1][c] + 1) >> 1;
  }
  for ( ; rp < ip[0]; rp+=4) {
    if (unique_id < 0x80000218) {
      pix[0] = rp[0] + rp[2] - 512;
      pix[2] = rp[0] + rp[1] - 512;
      pix[1] = rp[0] + ((-778*rp[1] - (rp[2] << 11)) >> 12) - 512;
    } else {
      rp[1] = (rp[1] << 2) + hue;
      rp[2] = (rp[2] << 2) + hue;
      pix[0] = rp[0] + ((   50*rp[1] + 22929*rp[2]) >> 14);
      pix[1] = rp[0] + ((-5640*rp[1] - 11751*rp[2]) >> 14);
      pix[2] = rp[0] + ((29040*rp[1] -   101*rp[2]) >> 14);
    }
    FORC3 rp[c] = CLIP(pix[c] * sraw_mul[c] >> 10);
  }
  ljpeg_end (&jh);
  maximum = 0x3fff;
}
