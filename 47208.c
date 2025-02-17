void CLASS adobe_copy_pixel (int row, int col, ushort **rp)
{
  unsigned r, c;

  r = row -= top_margin;
  c = col -= left_margin;
  if (is_raw == 2 && shot_select) (*rp)++;
  if (filters) {
    if (fuji_width) {
      r = row + fuji_width - 1 - (col >> 1);
      c = row + ((col+1) >> 1);
    }
    if (r < height && c < width)
      BAYER(r,c) = **rp < 0x1000 ? curve[**rp] : **rp;
    *rp += is_raw;
  } else {
    if (r < height && c < width)
      FORC(tiff_samples)
	image[row*width+col][c] = (*rp)[c] < 0x1000 ? curve[(*rp)[c]]:(*rp)[c];
    *rp += tiff_samples;
  }
  if (is_raw == 2 && shot_select) (*rp)--;
}
