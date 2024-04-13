static void RemapIndices(const ssize_t *map, const unsigned char *source,
  unsigned char *target)
{
  register ssize_t
    i;

  for (i = 0; i < 16; i++)
  {
    if (map[i] == -1)
      target[i] = 3;
    else
      target[i] = source[map[i]];
  }
}
