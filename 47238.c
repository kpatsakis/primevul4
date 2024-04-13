const char * CLASS foveon_camf_param (const char *block, const char *param)
{
  unsigned idx, num;
  char *pos, *cp, *dp;

  for (idx=0; idx < meta_length; idx += sget4(pos+8)) {
    pos = meta_data + idx;
    if (strncmp (pos, "CMb", 3)) break;
    if (pos[3] != 'P') continue;
    if (strcmp (block, pos+sget4(pos+12))) continue;
    cp = pos + sget4(pos+16);
    num = sget4(cp);
    dp = pos + sget4(cp+4);
    while (num--) {
      cp += 8;
      if (!strcmp (param, dp+sget4(cp)))
	return dp+sget4(cp+4);
    }
  }
  return 0;
}
