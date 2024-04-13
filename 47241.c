char * CLASS foveon_gets (int offset, char *str, int len)
{
  int i;
  fseek (ifp, offset, SEEK_SET);
  for (i=0; i < len-1; i++)
    if ((str[i] = get2()) == 0) break;
  str[i] = 0;
  return str;
}
