int msPostGISHexDecode(unsigned char *dest, const char *src, int srclen)
{

  if (src && *src && (srclen % 2 == 0) ) {

    unsigned char *p = dest;
    int i;

    for ( i=0; i<srclen; i+=2 ) {
      register unsigned char b1=0, b2=0;
      register unsigned char c1 = src[i];
      register unsigned char c2 = src[i + 1];

      b1 = msPostGISHexDecodeChar[c1];
      b2 = msPostGISHexDecodeChar[c2];

      *p++ = (b1 << 4) | b2;

    }
    return(p-dest);
  }
  return 0;
}
