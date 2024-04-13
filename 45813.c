arcStrokeCircularString(wkbObj *w, double segment_angle, lineObj *line)
{
  pointObj p1, p2, p3;
  int npoints, nedges;
  int edge = 0;
  pointArrayObj *pa;

  if ( ! w || ! line ) return MS_FAILURE;

  npoints = wkbReadInt(w);
  nedges = npoints / 2;

  /* All CircularStrings have an odd number of points */
  if ( npoints < 3 || npoints % 2 != 1 )
    return MS_FAILURE;

  /* Make a large guess at how much space we'll need */
  pa = pointArrayNew(nedges * 180 / segment_angle);

  wkbReadPointP(w,&p3);

  /* Fill out the point array with stroked arcs */
  while( edge < nedges ) {
    p1 = p3;
    wkbReadPointP(w,&p2);
    wkbReadPointP(w,&p3);
    if ( arcStrokeCircle(&p1, &p2, &p3, segment_angle, edge ? 0 : 1, pa) == MS_FAILURE ) {
      pointArrayFree(pa);
      return MS_FAILURE;
    }
    edge++;
  }

  /* Copy the point array into the line */
  line->numpoints = pa->npoints;
  line->point = msSmallMalloc(line->numpoints * sizeof(pointObj));
  memcpy(line->point, pa->data, line->numpoints * sizeof(pointObj));

  /* Clean up */
  pointArrayFree(pa);

  return MS_SUCCESS;
}
