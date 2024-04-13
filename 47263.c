void CLASS kodak_dc120_load_raw()
{
  static const int mul[4] = { 162, 192, 187,  92 };
  static const int add[4] = {   0, 636, 424, 212 };
  uchar pixel[848];
  int row, shift, col;

  for (row=0; row < height; row++) {
    if (fread (pixel, 1, 848, ifp) < 848) derror();
    shift = row * mul[row & 3] + add[row & 3];
    for (col=0; col < width; col++)
      BAYER(row,col) = (ushort) pixel[(col + shift) % 848];
  }
  maximum = 0xff;
}
