wkbReadLine(wkbObj *w, lineObj *line)
{
  int i;
  pointObj p;
  int npoints = wkbReadInt(w);

  line->numpoints = npoints;
  line->point = msSmallMalloc(npoints * sizeof(pointObj));
  for ( i = 0; i < npoints; i++ ) {
    wkbReadPointP(w, &p);
    line->point[i] = p;
  }
}
