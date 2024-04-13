static void ComputeWeightedCovariance(const size_t count,
  const DDSVector4 *points, float *covariance)
{
  DDSVector3
    centroid;

  float
    total;

  size_t
    i;

  total = 0.0f;
  VectorInit3(centroid,0.0f);

  for (i=0; i < count; i++)
  {
    total += points[i].w;
    centroid.x += (points[i].x * points[i].w);
    centroid.y += (points[i].y * points[i].w);
    centroid.z += (points[i].z * points[i].w);
  }

  if( total > 1.192092896e-07F)
    {
      centroid.x /= total;
      centroid.y /= total;
      centroid.z /= total;
    }

  for (i=0; i < 6; i++)
    covariance[i] = 0.0f;

  for (i = 0; i < count; i++)
  {
    DDSVector3
      a,
      b;

    a.x = points[i].x - centroid.x;
    a.y = points[i].y - centroid.y;
    a.z = points[i].z - centroid.z;

    b.x = points[i].w * a.x;
    b.y = points[i].w * a.y;
    b.z = points[i].w * a.z;

    covariance[0] += a.x*b.x;
    covariance[1] += a.x*b.y;
    covariance[2] += a.x*b.z;
    covariance[3] += a.y*b.y;
    covariance[4] += a.y*b.z;
    covariance[5] += a.z*b.z;
  }
}
