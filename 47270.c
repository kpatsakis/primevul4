void CLASS leaf_hdr_load_raw()
{
  ushort *pixel;
  unsigned tile=0, r, c, row, col;

  pixel = (ushort *) calloc (raw_width, sizeof *pixel);
  merror (pixel, "leaf_hdr_load_raw()");
  FORC(tiff_samples)
    for (r=0; r < raw_height; r++) {
      if (r % tile_length == 0) {
	fseek (ifp, data_offset + 4*tile++, SEEK_SET);
	fseek (ifp, get4() + 2*left_margin, SEEK_SET);
      }
      if (filters && c != shot_select) continue;
      read_shorts (pixel, raw_width);
      if ((row = r - top_margin) >= height) continue;
      for (col=0; col < width; col++)
	if (filters)  BAYER(row,col) = pixel[col];
	else image[row*width+col][c] = pixel[col];
    }
  free (pixel);
  if (!filters) {
    maximum = 0xffff;
    raw_color = 1;
  }
}
