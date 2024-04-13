void * CLASS foveon_camf_matrix (unsigned dim[3], const char *name)
{
  unsigned i, idx, type, ndim, size, *mat;
  char *pos, *cp, *dp;
  double dsize;

  for (idx=0; idx < meta_length; idx += sget4(pos+8)) {
    pos = meta_data + idx;
    if (strncmp (pos, "CMb", 3)) break;
    if (pos[3] != 'M') continue;
    if (strcmp (name, pos+sget4(pos+12))) continue;
    dim[0] = dim[1] = dim[2] = 1;
    cp = pos + sget4(pos+16);
    type = sget4(cp);
    if ((ndim = sget4(cp+4)) > 3) break;
    dp = pos + sget4(cp+8);
    for (i=ndim; i--; ) {
      cp += 12;
      dim[i] = sget4(cp);
    }
    if ((dsize = (double) dim[0]*dim[1]*dim[2]) > meta_length/4) break;
    mat = (unsigned *) malloc ((size = dsize) * 4);
    merror (mat, "foveon_camf_matrix()");
    for (i=0; i < size; i++)
      if (type && type != 6)
	mat[i] = sget4(dp + i*4);
      else
	mat[i] = sget4(dp + i*2) & 0xffff;
    return mat;
  }
  dcraw_message (DCRAW_WARNING,_("%s: \"%s\" matrix not found!\n"), ifname_display, name);
  return 0;
}
