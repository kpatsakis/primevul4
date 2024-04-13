static MagickBooleanType ConstructOrdering(const size_t count,
  const DDSVector4 *points, const DDSVector3 axis, DDSVector4 *pointsWeights,
  DDSVector4 *xSumwSum, unsigned char *order, size_t iteration)
{
  float
     dps[16],
     f;

  register ssize_t
    i;

  size_t
    j;

  unsigned char
    c,
    *o,
    *p;

  o = order + (16*iteration);

  for (i=0; i < (ssize_t) count; i++)
  {
    dps[i] = Dot(points[i],axis);
    o[i] = (unsigned char)i;
  }

  for (i=0; i < (ssize_t) count; i++)
  {
    for (j=i; j > 0 && dps[j] < dps[j - 1]; j--)
    {
      f = dps[j];
      dps[j] = dps[j - 1];
      dps[j - 1] = f;

      c = o[j];
      o[j] = o[j - 1];
      o[j - 1] = c;
    }
  }

  for (i=0; i < (ssize_t) iteration; i++)
  {
    MagickBooleanType
      same;

    p = order + (16*i);
    same = MagickTrue;

    for (j=0; j < count; j++)
    {
      if (o[j] != p[j])
        {
          same = MagickFalse;
          break;
        }
    }

    if (same != MagickFalse)
      return MagickFalse;
  }

  xSumwSum->x = 0;
  xSumwSum->y = 0;
  xSumwSum->z = 0;
  xSumwSum->w = 0;

  for (i=0; i < (ssize_t) count; i++)
  {
    DDSVector4
      v;

    j = (size_t) o[i];

    v.x = points[j].w * points[j].x;
    v.y = points[j].w * points[j].y;
    v.z = points[j].w * points[j].z;
    v.w = points[j].w * 1.0f;

    VectorCopy44(v,&pointsWeights[i]);
    VectorAdd(*xSumwSum,v,xSumwSum);
  }

  return MagickTrue;
}
