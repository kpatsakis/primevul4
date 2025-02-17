void CLASS sony_load_raw()
{
  uchar head[40];
  ushort *pixel;
  unsigned i, key, row, col;

  fseek (ifp, 200896, SEEK_SET);
  fseek (ifp, (unsigned) fgetc(ifp)*4 - 1, SEEK_CUR);
  order = 0x4d4d;
  key = get4();
  fseek (ifp, 164600, SEEK_SET);
  fread (head, 1, 40, ifp);
  sony_decrypt ((unsigned int *) head, 10, 1, key);
  for (i=26; i-- > 22; )
    key = key << 8 | head[i];
  fseek (ifp, data_offset, SEEK_SET);
  pixel = (ushort *) calloc (raw_width, sizeof *pixel);
  merror (pixel, "sony_load_raw()");
  for (row=0; row < height; row++) {
    if (fread (pixel, 2, raw_width, ifp) < raw_width) derror();
    sony_decrypt ((unsigned int *) pixel, raw_width/2, !row, key);
    for (col=9; col < left_margin; col++)
      black += ntohs(pixel[col]);
    for (col=0; col < width; col++)
      if ((BAYER(row,col) = ntohs(pixel[col+left_margin])) >> 14)
	derror();
  }
  free (pixel);
  if (left_margin > 9)
    black /= (left_margin-9) * height;
  maximum = 0x3ff0;
}
