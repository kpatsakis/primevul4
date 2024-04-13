void CLASS sinar_4shot_load_raw()
{
  ushort *pixel;
  unsigned shot, row, col, r, c;

  if ((shot = shot_select) || half_size) {
    if (shot) shot--;
    if (shot > 3) shot = 3;
    fseek (ifp, data_offset + shot*4, SEEK_SET);
    fseek (ifp, get4(), SEEK_SET);
    unpacked_load_raw();
    return;
  }
  free (image);
  image = (ushort (*)[4])
	calloc ((iheight=height)*(iwidth=width), sizeof *image);
  merror (image, "sinar_4shot_load_raw()");
  pixel = (ushort *) calloc (raw_width, sizeof *pixel);
  merror (pixel, "sinar_4shot_load_raw()");
  for (shot=0; shot < 4; shot++) {
    fseek (ifp, data_offset + shot*4, SEEK_SET);
    fseek (ifp, get4(), SEEK_SET);
    for (row=0; row < raw_height; row++) {
      read_shorts (pixel, raw_width);
      if ((r = row-top_margin - (shot >> 1 & 1)) >= height) continue;
      for (col=0; col < raw_width; col++) {
	if ((c = col-left_margin - (shot & 1)) >= width) continue;
	image[r*width+c][FC(row,col)] = pixel[col];
      }
    }
  }
  free (pixel);
  shrink = filters = 0;
}
