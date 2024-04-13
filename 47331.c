void CLASS smal_v6_load_raw()
{
  unsigned seg[2][2];

  fseek (ifp, 16, SEEK_SET);
  seg[0][0] = 0;
  seg[0][1] = get2();
  seg[1][0] = raw_width * raw_height;
  seg[1][1] = INT_MAX;
  smal_decode_segment (seg, 0);
}
