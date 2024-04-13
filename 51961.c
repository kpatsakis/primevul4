static PolygonInfo *DestroyPolygonInfo(PolygonInfo *polygon_info)
{
  register ssize_t
    i;

  for (i=0; i < (ssize_t) polygon_info->number_edges; i++)
    polygon_info->edges[i].points=(PointInfo *)
      RelinquishMagickMemory(polygon_info->edges[i].points);
  polygon_info->edges=(EdgeInfo *) RelinquishMagickMemory(polygon_info->edges);
  return((PolygonInfo *) RelinquishMagickMemory(polygon_info));
}
