void CLASS canon_600_auto_wb()
{
  int mar, row, col, i, j, st, count[] = { 0,0 };
  int test[8], total[2][8], ratio[2][2], stat[2];

  memset (&total, 0, sizeof total);
  i = canon_ev + 0.5;
  if      (i < 10) mar = 150;
  else if (i > 12) mar = 20;
  else mar = 280 - 20 * i;
  if (flash_used) mar = 80;
  for (row=14; row < height-14; row+=4)
    for (col=10; col < width; col+=2) {
      for (i=0; i < 8; i++)
	test[(i & 4) + FC(row+(i >> 1),col+(i & 1))] =
		    BAYER(row+(i >> 1),col+(i & 1));
      for (i=0; i < 8; i++)
	if (test[i] < 150 || test[i] > 1500) goto next;
      for (i=0; i < 4; i++)
	if (abs(test[i] - test[i+4]) > 50) goto next;
      for (i=0; i < 2; i++) {
	for (j=0; j < 4; j+=2)
	  ratio[i][j >> 1] = ((test[i*4+j+1]-test[i*4+j]) << 10) / test[i*4+j];
	stat[i] = canon_600_color (ratio[i], mar);
      }
      if ((st = stat[0] | stat[1]) > 1) goto next;
      for (i=0; i < 2; i++)
	if (stat[i])
	  for (j=0; j < 2; j++)
	    test[i*4+j*2+1] = test[i*4+j*2] * (0x400 + ratio[i][j]) >> 10;
      for (i=0; i < 8; i++)
	total[st][i] += test[i];
      count[st]++;
next: ;
    }
  if (count[0] | count[1]) {
    st = count[0]*200 < count[1];
    for (i=0; i < 4; i++)
      pre_mul[i] = 1.0 / (total[st][i] + total[st][i+4]);
  }
}
