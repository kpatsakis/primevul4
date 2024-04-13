void CLASS sony_arw_load_raw()
{
  ushort huff[32768];
  static const ushort tab[18] =
  { 0xf11,0xf10,0xe0f,0xd0e,0xc0d,0xb0c,0xa0b,0x90a,0x809,
    0x708,0x607,0x506,0x405,0x304,0x303,0x300,0x202,0x201 };
  int i, c, n, col, row, len, diff, sum=0;

  for (n=i=0; i < 18; i++)
    FORC(32768 >> (tab[i] >> 8)) huff[n++] = tab[i];
  getbits(-1);
  for (col = raw_width; col--; )
    for (row=0; row < raw_height+1; row+=2) {
      if (row == raw_height) row = 1;
      len = getbithuff(15,huff);
      diff = getbits(len);
      if ((diff & (1 << (len-1))) == 0)
	diff -= (1 << len) - 1;
      if ((sum += diff) >> 12) derror();
      if (row < height) BAYER(row,col) = sum;
    }
}
