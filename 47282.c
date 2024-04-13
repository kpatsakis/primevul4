void CLASS minolta_rd175_load_raw()
{
  uchar pixel[768];
  unsigned irow, box, row, col;

  for (irow=0; irow < 1481; irow++) {
    if (fread (pixel, 1, 768, ifp) < 768) derror();
    box = irow / 82;
    row = irow % 82 * 12 + ((box < 12) ? box | 1 : (box-12)*2);
    switch (irow) {
      case 1477: case 1479: continue;
      case 1476: row = 984; break;
      case 1480: row = 985; break;
      case 1478: row = 985; box = 1;
    }
    if ((box < 12) && (box & 1)) {
      for (col=0; col < 1533; col++, row ^= 1)
	if (col != 1) BAYER(row,col) = (col+1) & 2 ?
		   pixel[col/2-1] + pixel[col/2+1] : pixel[col/2] << 1;
      BAYER(row,1)    = pixel[1]   << 1;
      BAYER(row,1533) = pixel[765] << 1;
    } else
      for (col=row & 1; col < 1534; col+=2)
	BAYER(row,col) = pixel[col/2] << 1;
  }
  maximum = 0xff << 1;
}
