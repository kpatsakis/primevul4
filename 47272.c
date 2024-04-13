void CLASS linear_table (unsigned len)
{
  int i;
  if (len > 0x1000) len = 0x1000;
  read_shorts (curve, len);
  for (i=len; i < 0x1000; i++)
    curve[i] = curve[i-1];
  maximum = curve[0xfff];
}
