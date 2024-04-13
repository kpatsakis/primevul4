void CLASS hasselblad_load_raw()
{
  struct jhead jh;
  int row, col, pred[2], len[2], diff, c;

  if (!ljpeg_start (&jh, 0)) return;
  order = 0x4949;
  ph1_bits(-1);
  for (row=-top_margin; row < height; row++) {
    pred[0] = pred[1] = 0x8000 + load_flags;
    for (col=-left_margin; col < raw_width-left_margin; col+=2) {
      FORC(2) len[c] = ph1_huff(jh.huff[0]);
      FORC(2) {
	diff = ph1_bits(len[c]);
	if ((diff & (1 << (len[c]-1))) == 0)
	  diff -= (1 << len[c]) - 1;
	if (diff == 65535) diff = -32768;
	pred[c] += diff;
	if (row >= 0 && (unsigned)(col+c) < width)
	  BAYER(row,col+c) = pred[c];
      }
    }
  }
  ljpeg_end (&jh);
  maximum = 0xffff;
}
