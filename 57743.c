static size_t CompressAlpha(const size_t min, const size_t max,
  const size_t steps, const ssize_t *alphas, unsigned char* indices)
{
  unsigned char
    codes[8];

  register ssize_t
    i;

  size_t
    error,
    index,
    j,
    least,
    value;

  codes[0] = (unsigned char) min;
  codes[1] = (unsigned char) max;
  codes[6] = 0;
  codes[7] = 255;

  for (i=1; i < (ssize_t) steps; i++)
    codes[i+1] = (unsigned char) (((steps-i)*min + i*max) / steps);

  error = 0;
  for (i=0; i<16; i++)
  {
    if (alphas[i] == -1)
      {
        indices[i] = 0;
        continue;
      }

    value = alphas[i];
    least = SIZE_MAX;
    index = 0;
    for (j=0; j<8; j++)
    {
      size_t
        dist;

      dist = value - (size_t)codes[j];
      dist *= dist;

      if (dist < least)
        {
          least = dist;
          index = j;
        }
    }

    indices[i] = (unsigned char)index;
    error += least;
  }

  return error;
}
