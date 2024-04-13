static inline void hex_encode(unsigned char *str, int len, unsigned char *out)
{
   int i;
   for (i = 0; i < len; ++i) {
      out[0] = itoa16[str[i]>>4];
      out[1] = itoa16[str[i]&0xF];
      out += 2;
   }
}
