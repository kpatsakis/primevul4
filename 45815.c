int msPostGISBase64Decode(unsigned char *dest, const char *src, int srclen)
{

  if (src && *src) {

    unsigned char *p = dest;
    int i, j, k;
    unsigned char *buf = calloc(srclen + 1, sizeof(unsigned char));

    /* Drop illegal chars first */
    for (i=0, j=0; src[i]; i++) {
      unsigned char c = src[i];
      if ( (msPostGISBase64DecodeChar[c] != 64) || (c == '=') ) {
        buf[j++] = c;
      }
    }

    for (k=0; k<j; k+=4) {
      register unsigned char c1='A', c2='A', c3='A', c4='A';
      register unsigned char b1=0, b2=0, b3=0, b4=0;

      c1 = buf[k];

      if (k+1<j) {
        c2 = buf[k+1];
      }
      if (k+2<j) {
        c3 = buf[k+2];
      }
      if (k+3<j) {
        c4 = buf[k+3];
      }

      b1 = msPostGISBase64DecodeChar[c1];
      b2 = msPostGISBase64DecodeChar[c2];
      b3 = msPostGISBase64DecodeChar[c3];
      b4 = msPostGISBase64DecodeChar[c4];

      *p++=((b1<<2)|(b2>>4) );
      if (c3 != '=') {
        *p++=(((b2&0xf)<<4)|(b3>>2) );
      }
      if (c4 != '=') {
        *p++=(((b3&0x3)<<6)|b4 );
      }
    }
    free(buf);
    return(p-dest);
  }
  return 0;
}
