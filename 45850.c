wkbConvCurvePolygonToShape(wkbObj *w, shapeObj *shape)
{
  int type, i, ncomponents;
  int failures = 0;
  int was_poly = ( shape->type == MS_SHAPE_POLYGON );

  /*endian = */wkbReadChar(w);
  type = wkbTypeMap(w,wkbReadInt(w));
  ncomponents = wkbReadInt(w);

  if( type != WKB_CURVEPOLYGON ) return MS_FAILURE;

  /* Lower the allowed dimensionality so we can
  *  catch the linear ring components */
  shape->type = MS_SHAPE_LINE;

  for ( i = 0; i < ncomponents; i++ ) {
    if ( wkbConvGeometryToShape(w, shape) == MS_FAILURE ) {
      wkbSkipGeometry(w);
      failures++;
    }
  }

  /* Go back to expected dimensionality */
  if ( was_poly) shape->type = MS_SHAPE_POLYGON;

  if ( failures == ncomponents )
    return MS_FAILURE;
  else
    return MS_SUCCESS;
}
