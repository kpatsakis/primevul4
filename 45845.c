pointArrayNew(int maxpoints)
{
  pointArrayObj *d = msSmallMalloc(sizeof(pointArrayObj));
  if ( maxpoints < 1 ) maxpoints = 1; /* Avoid a degenerate case */
  d->maxpoints = maxpoints;
  d->data = msSmallMalloc(maxpoints * sizeof(pointObj));
  d->npoints = 0;
  return d;
}
