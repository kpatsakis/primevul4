void CLASS phase_one_correct()
{
  unsigned entries, tag, data, save, col, row, type;
  int len, i, j, k, cip, val[4], dev[4], sum, max;
  int head[9], diff, mindiff=INT_MAX, off_412=0;
  static const signed char dir[12][2] =
    { {-1,-1}, {-1,1}, {1,-1}, {1,1}, {-2,0}, {0,-2}, {0,2}, {2,0},
      {-2,-2}, {-2,2}, {2,-2}, {2,2} };
  float poly[8], num, cfrac, frac, mult[2], *yval[2];
  ushort *xval[2];

  if (half_size || !meta_length) return;
  dcraw_message (DCRAW_VERBOSE,_("Phase One correction...\n"));
  fseek (ifp, meta_offset, SEEK_SET);
  order = get2();
  fseek (ifp, 6, SEEK_CUR);
  fseek (ifp, meta_offset+get4(), SEEK_SET);
  entries = get4();  get4();
  while (entries--) {
    tag  = get4();
    len  = get4();
    data = get4();
    save = ftell(ifp);
    fseek (ifp, meta_offset+data, SEEK_SET);
    if (tag == 0x419) {				/* Polynomial curve */
      for (get4(), i=0; i < 8; i++)
	poly[i] = getreal(11);
      poly[3] += (ph1.tag_210 - poly[7]) * poly[6] + 1;
      for (i=0; i < 0x10000; i++) {
	num = (poly[5]*i + poly[3])*i + poly[1];
	curve[i] = LIM(num,0,65535);
      } goto apply;				/* apply to right half */
    } else if (tag == 0x41a) {			/* Polynomial curve */
      for (i=0; i < 4; i++)
	poly[i] = getreal(11);
      for (i=0; i < 0x10000; i++) {
	for (num=0, j=4; j--; )
	  num = num * i + poly[j];
	curve[i] = LIM(num+i,0,65535);
      } apply:					/* apply to whole image */
      for (row=0; row < height; row++)
	for (col = (tag & 1)*ph1.split_col; col < width; col++)
	  BAYER(row,col) = curve[BAYER(row,col)];
    } else if (tag == 0x400) {			/* Sensor defects */
      while ((len -= 8) >= 0) {
	col  = get2() - left_margin;
	row  = get2() - top_margin;
	type = get2(); get2();
	if (col >= width) continue;
	if (type == 131)			/* Bad column */
	  for (row=0; row < height; row++)
	    if (FC(row,col) == 1) {
	      for (sum=i=0; i < 4; i++)
		sum += val[i] = bayer (row+dir[i][0], col+dir[i][1]);
	      for (max=i=0; i < 4; i++) {
		dev[i] = abs((val[i] << 2) - sum);
		if (dev[max] < dev[i]) max = i;
	      }
	      BAYER(row,col) = (sum - val[max])/3.0 + 0.5;
	    } else {
	      for (sum=0, i=8; i < 12; i++)
		sum += bayer (row+dir[i][0], col+dir[i][1]);
	      BAYER(row,col) = 0.5 + sum * 0.0732233 +
		(bayer(row,col-2) + bayer(row,col+2)) * 0.3535534;
	    }
	else if (type == 129) {			/* Bad pixel */
	  if (row >= height) continue;
	  j = (FC(row,col) != 1) * 4;
	  for (sum=0, i=j; i < j+8; i++)
	    sum += bayer (row+dir[i][0], col+dir[i][1]);
	  BAYER(row,col) = (sum + 4) >> 3;
	}
      }
    } else if (tag == 0x401) {			/* All-color flat fields */
      phase_one_flat_field (1, 2);
    } else if (tag == 0x416 || tag == 0x410) {
      phase_one_flat_field (0, 2);
    } else if (tag == 0x40b) {			/* Red+blue flat field */
      phase_one_flat_field (0, 4);
    } else if (tag == 0x412) {
      fseek (ifp, 36, SEEK_CUR);
      diff = abs (get2() - ph1.tag_21a);
      if (mindiff > diff) {
	mindiff = diff;
	off_412 = ftell(ifp) - 38;
      }
    }
    fseek (ifp, save, SEEK_SET);
  }
  if (off_412) {
    fseek (ifp, off_412, SEEK_SET);
    for (i=0; i < 9; i++) head[i] = get4() & 0x7fff;
    yval[0] = (float *) calloc (head[1]*head[3] + head[2]*head[4], 6);
    merror (yval[0], "phase_one_correct()");
    yval[1] = (float  *) (yval[0] + head[1]*head[3]);
    xval[0] = (ushort *) (yval[1] + head[2]*head[4]);
    xval[1] = (ushort *) (xval[0] + head[1]*head[3]);
    get2();
    for (i=0; i < 2; i++)
      for (j=0; j < head[i+1]*head[i+3]; j++)
	yval[i][j] = getreal(11);
    for (i=0; i < 2; i++)
      for (j=0; j < head[i+1]*head[i+3]; j++)
	xval[i][j] = get2();
    for (row=0; row < height; row++)
      for (col=0; col < width; col++) {
	cfrac = (float) col * head[3] / raw_width;
	cfrac -= cip = cfrac;
	num = BAYER(row,col) * 0.5;
	for (i=cip; i < cip+2; i++) {
	  for (k=j=0; j < head[1]; j++)
	    if (num < xval[0][k = head[1]*i+j]) break;
	  frac = (j == 0 || j == head[1]) ? 0 :
		(xval[0][k] - num) / (xval[0][k] - xval[0][k-1]);
	  mult[i-cip] = yval[0][k-1] * frac + yval[0][k] * (1-frac);
	}
	i = ((mult[0] * (1-cfrac) + mult[1] * cfrac)
		* (row + top_margin) + num) * 2;
	BAYER(row,col) = LIM(i,0,65535);
      }
    free (yval[0]);
  }
}
