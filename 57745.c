static void CompressRangeFit(const size_t count,
  const DDSVector4 *points, const ssize_t *map, const DDSVector3 principle,
  const DDSVector4 metric, DDSVector3 *start, DDSVector3 *end,
  unsigned char *indices)
{
  float
    d,
    bestDist,
    max,
    min,
    val;

  DDSVector3
    codes[4],
    grid,
    gridrcp,
    half,
    dist;

  register ssize_t
    i;

  size_t
    bestj,
    j;

  unsigned char
    closest[16];

  VectorInit3(half,0.5f);

  grid.x = 31.0f;
  grid.y = 63.0f;
  grid.z = 31.0f;

  gridrcp.x = 1.0f/31.0f;
  gridrcp.y = 1.0f/63.0f;
  gridrcp.z = 1.0f/31.0f;

  if (count > 0)
    {
      VectorCopy43(points[0],start);
      VectorCopy43(points[0],end);

      min = max = Dot(points[0],principle);
      for (i=1; i < (ssize_t) count; i++)
      {
        val = Dot(points[i],principle);
        if (val < min)
        {
          VectorCopy43(points[i],start);
          min = val;
        }
        else if (val > max)
        {
          VectorCopy43(points[i],end);
          max = val;
        }
      }
    }

  VectorClamp3(start);
  VectorMultiplyAdd3(grid,*start,half,start);
  VectorTruncate3(start);
  VectorMultiply3(*start,gridrcp,start);

  VectorClamp3(end);
  VectorMultiplyAdd3(grid,*end,half,end);
  VectorTruncate3(end);
  VectorMultiply3(*end,gridrcp,end);

  codes[0] = *start;
  codes[1] = *end;
  codes[2].x = (start->x * (2.0f/3.0f)) + (end->x * (1.0f/3.0f));
  codes[2].y = (start->y * (2.0f/3.0f)) + (end->y * (1.0f/3.0f));
  codes[2].z = (start->z * (2.0f/3.0f)) + (end->z * (1.0f/3.0f));
  codes[3].x = (start->x * (1.0f/3.0f)) + (end->x * (2.0f/3.0f));
  codes[3].y = (start->y * (1.0f/3.0f)) + (end->y * (2.0f/3.0f));
  codes[3].z = (start->z * (1.0f/3.0f)) + (end->z * (2.0f/3.0f));

  for (i=0; i < (ssize_t) count; i++)
  {
    bestDist = 1e+37f;
    bestj = 0;
    for (j=0; j < 4; j++)
    {
      dist.x = (points[i].x - codes[j].x) * metric.x;
      dist.y = (points[i].y - codes[j].y) * metric.y;
      dist.z = (points[i].z - codes[j].z) * metric.z;

      d = Dot(dist,dist);
      if (d < bestDist)
        {
          bestDist = d;
          bestj = j;
        }
    }

    closest[i] = (unsigned char) bestj;
  }

  RemapIndices(map, closest, indices);
}
