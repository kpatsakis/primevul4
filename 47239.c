void CLASS foveon_decoder (unsigned size, unsigned code)
{
  static unsigned huff[1024];
  struct decode *cur;
  unsigned i, len;

  if (!code) {
    for (i=0; i < size; i++)
      huff[i] = get4();
    memset (first_decode, 0, sizeof first_decode);
    free_decode = first_decode;
  }
  cur = free_decode++;
  if (free_decode > first_decode+2048) {
    dcraw_message (DCRAW_ERROR,_("%s: decoder table overflow\n"), ifname_display);
    longjmp (failure, 2);
  }
  if (code)
    for (i=0; i < size; i++)
      if (huff[i] == code) {
	cur->leaf = i;
	return;
      }
  if ((len = code >> 27) > 26) return;
  code = (len+1) << 27 | (code & 0x3ffffff) << 1;

  cur->branch[0] = free_decode;
  foveon_decoder (size, code);
  cur->branch[1] = free_decode;
  foveon_decoder (size, code+1);
}
