pointArrayFree(pointArrayObj *d)
{
  if ( ! d ) return;
  if ( d->data ) free(d->data);
  free(d);
}
