void CLASS fill_holes (int holes)
{
  int row, col, val[4];

  for (row=2; row < height-2; row++) {
    if (!HOLE(row)) continue;
    for (col=1; col < width-1; col+=4) {
      val[0] = BAYER(row-1,col-1);
      val[1] = BAYER(row-1,col+1);
      val[2] = BAYER(row+1,col-1);
      val[3] = BAYER(row+1,col+1);
      BAYER(row,col) = median4(val);
    }
    for (col=2; col < width-2; col+=4)
      if (HOLE(row-2) || HOLE(row+2))
	BAYER(row,col) = (BAYER(row,col-2) + BAYER(row,col+2)) >> 1;
      else {
	val[0] = BAYER(row,col-2);
	val[1] = BAYER(row,col+2);
	val[2] = BAYER(row-2,col);
	val[3] = BAYER(row+2,col);
	BAYER(row,col) = median4(val);
      }
  }
}
