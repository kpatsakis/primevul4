pointArrayAddPoint(pointArrayObj *d, const pointObj *p)
{
  if ( !p || !d ) return MS_FAILURE;
  /* Avoid overwriting memory buffer */
  if ( d->maxpoints - d->npoints == 0 ) {
    d->maxpoints *= 2;
    d->data = realloc(d->data, d->maxpoints * sizeof(pointObj));
  }
  d->data[d->npoints] = *p;
  d->npoints++;
  return MS_SUCCESS;
}
