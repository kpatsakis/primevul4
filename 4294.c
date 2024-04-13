mrb_bidx(uint16_t c)
{
  uint8_t n = c & 0xf;
  uint8_t k = (c>>4) & 0xf;
  if (n == 15) n = 1;
  if (k == 15) n += 1;
  else n += k*2;
  return n + 1;                 /* self + args + kargs */
}