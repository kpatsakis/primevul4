static void WriteCompressed(Image *image, const size_t count,
  DDSVector4* points, const ssize_t* map, const MagickBooleanType clusterFit)
{
  float
    covariance[16];

  DDSVector3
    end,
    principle,
    start;

  DDSVector4
    metric;

  unsigned char
    indices[16];

  VectorInit(metric,1.0f);
  VectorInit3(start,0.0f);
  VectorInit3(end,0.0f);

  ComputeWeightedCovariance(count,points,covariance);
  ComputePrincipleComponent(covariance,&principle);

  if (clusterFit == MagickFalse || count == 0)
    CompressRangeFit(count,points,map,principle,metric,&start,&end,indices);
  else
    CompressClusterFit(count,points,map,principle,metric,&start,&end,indices);

  WriteIndices(image,start,end,indices);
}
