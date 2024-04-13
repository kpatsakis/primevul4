wkbConvCollectionToShape(wkbObj *w, shapeObj *shape)
{
  int i, ncomponents;
  int failures = 0;

  /*endian = */wkbReadChar(w);
  /*type = */wkbTypeMap(w,wkbReadInt(w));
  ncomponents = wkbReadInt(w);

  /*
  * If we can draw any portion of the collection, we will,
  * but if all the components fail, we will draw nothing.
  */
  for ( i = 0; i < ncomponents; i++ ) {
    if ( wkbConvGeometryToShape(w, shape) == MS_FAILURE ) {
      wkbSkipGeometry(w);
      failures++;
    }
  }
  if ( failures == ncomponents )
    return MS_FAILURE;
  else
    return MS_SUCCESS;
}
