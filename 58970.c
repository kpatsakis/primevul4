void CLASS foveon_huff (ushort *huff)
{
  int i, j, clen, code;

  huff[0] = 8;
  for (i=0; i < 13; i++) {
    clen = getc(ifp);
    code = getc(ifp);
    for (j=0; j < 256 >> clen; )
      huff[code+ ++j] = clen << 8 | i;
  }
  get2();
}
