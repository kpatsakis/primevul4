void CLASS nokia_load_raw()
{
  uchar  *data,  *dp;
  ushort *pixel, *pix;
  int rev, dwide, row, c;

  rev = 3 * (order == 0x4949);
  dwide = raw_width * 5 / 4;
  data = (uchar *) malloc (dwide + raw_width*2);
  merror (data, "nokia_load_raw()");
  pixel = (ushort *) (data + dwide);
  for (row=0; row < raw_height; row++) {
    if ((int) fread (data+dwide, 1, dwide, ifp) < dwide) derror();
    FORC(dwide) data[c] = data[dwide+(c ^ rev)];
    for (dp=data, pix=pixel; pix < pixel+raw_width; dp+=5, pix+=4)
      FORC4 pix[c] = (dp[c] << 2) | (dp[4] >> (c << 1) & 3);
    if (row < top_margin)
      FORC(width) black += pixel[c];
    else
      FORC(width) BAYER(row-top_margin,c) = pixel[c];
  }
  free (data);
  if (top_margin) black /= top_margin * width;
  maximum = 0x3ff;
}
