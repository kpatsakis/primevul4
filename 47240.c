int CLASS foveon_fixed (void *ptr, int size, const char *name)
{
  void *dp;
  unsigned dim[3];

  dp = foveon_camf_matrix (dim, name);
  if (!dp) return 0;
  memcpy (ptr, dp, size*4);
  free (dp);
  return 1;
}
