static void WriteSingleColorFit(Image *image, const DDSVector4* points,
  const ssize_t* map)
{
  DDSVector3
    start,
    end;

  register ssize_t
    i;

  unsigned char
    color[3],
    index,
    indexes[16],
    indices[16];

  color[0] = (unsigned char) ClampToLimit(255.0f*points->x,255);
  color[1] = (unsigned char) ClampToLimit(255.0f*points->y,255);
  color[2] = (unsigned char) ClampToLimit(255.0f*points->z,255);

  index=0;
  ComputeEndPoints(DDS_LOOKUP,color,&start,&end,&index);

  for (i=0; i< 16; i++)
    indexes[i]=index;
  RemapIndices(map,indexes,indices);
  WriteIndices(image,start,end,indices);
}
