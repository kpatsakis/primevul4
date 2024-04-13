static void LogPolygonInfo(const PolygonInfo *polygon_info)
{
  register EdgeInfo
    *p;

  register ssize_t
    i,
    j;

  (void) LogMagickEvent(DrawEvent,GetMagickModule(),"    begin active-edge");
  p=polygon_info->edges;
  for (i=0; i < (ssize_t) polygon_info->number_edges; i++)
  {
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"      edge %.20g:",
      (double) i);
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"      direction: %s",
      p->direction != MagickFalse ? "down" : "up");
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"      ghostline: %s",
      p->ghostline != MagickFalse ? "transparent" : "opaque");
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),
      "      bounds: %g %g - %g %g",p->bounds.x1,p->bounds.y1,
      p->bounds.x2,p->bounds.y2);
    for (j=0; j < (ssize_t) p->number_points; j++)
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),"        %g %g",
        p->points[j].x,p->points[j].y);
    p++;
  }
  (void) LogMagickEvent(DrawEvent,GetMagickModule(),"    end active-edge");
}
