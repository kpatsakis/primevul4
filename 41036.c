get_random()
{
  long int l;
  prng_bytes ((unsigned char *)&l, sizeof(l));
  if (l < 0)
    l = -l;
  return l;
}
