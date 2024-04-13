static void ComputeEndPoints(const DDSSingleColourLookup *lookup[],
  const unsigned char *color, DDSVector3 *start, DDSVector3 *end,
  unsigned char *index)
{
  register ssize_t
    i;

  size_t
    c,
    maxError = SIZE_MAX;

  for (i=0; i < 2; i++)
  {
    const DDSSourceBlock*
      sources[3];

      size_t
        error = 0;

    for (c=0; c < 3; c++)
    {
      sources[c] = &lookup[c][color[c]].sources[i];
      error += ((size_t) sources[c]->error) * ((size_t) sources[c]->error);
    }

    if (error > maxError)
      continue;

    start->x = (float) sources[0]->start / 31.0f;
    start->y = (float) sources[1]->start / 63.0f;
    start->z = (float) sources[2]->start / 31.0f;

    end->x = (float) sources[0]->end / 31.0f;
    end->y = (float) sources[1]->end / 63.0f;
    end->z = (float) sources[2]->end / 31.0f;

    *index = (unsigned char) (2*i);
    maxError = error;
  }
}
