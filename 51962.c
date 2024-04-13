static PolygonInfo **DestroyPolygonThreadSet(PolygonInfo **polygon_info)
{
  register ssize_t
    i;

  assert(polygon_info != (PolygonInfo **) NULL);
  for (i=0; i < (ssize_t) GetMagickResourceLimit(ThreadResource); i++)
    if (polygon_info[i] != (PolygonInfo *) NULL)
      polygon_info[i]=DestroyPolygonInfo(polygon_info[i]);
  polygon_info=(PolygonInfo **) RelinquishMagickMemory(polygon_info);
  return(polygon_info);
}
