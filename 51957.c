static int CompareEdges(const void *x,const void *y)
{
  register const EdgeInfo
    *p,
    *q;

  /*
    Compare two edges.
  */
  p=(const EdgeInfo *) x;
  q=(const EdgeInfo *) y;
  if ((p->points[0].y-MagickEpsilon) > q->points[0].y)
    return(1);
  if ((p->points[0].y+MagickEpsilon) < q->points[0].y)
    return(-1);
  if ((p->points[0].x-MagickEpsilon) > q->points[0].x)
    return(1);
  if ((p->points[0].x+MagickEpsilon) < q->points[0].x)
    return(-1);
  if (((p->points[1].x-p->points[0].x)*(q->points[1].y-q->points[0].y)-
       (p->points[1].y-p->points[0].y)*(q->points[1].x-q->points[0].x)) > 0.0)
    return(1);
  return(-1);
}
