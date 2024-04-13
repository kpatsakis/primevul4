static void TraceArc(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo degrees)
{
  PointInfo
    center,
    radii;

  center.x=0.5*(end.x+start.x);
  center.y=0.5*(end.y+start.y);
  radii.x=fabs(center.x-start.x);
  radii.y=fabs(center.y-start.y);
  TraceEllipse(primitive_info,center,radii,degrees);
}
