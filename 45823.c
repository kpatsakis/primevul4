msPostGISFindBestType(wkbObj *w, shapeObj *shape)
{
  int wkbtype;

  /* What kind of geometry is this? */
  wkbtype = wkbType(w);

  /* Generic collection, we need to look a little deeper. */
  if ( wkbtype == WKB_GEOMETRYCOLLECTION )
    wkbtype = wkbCollectionSubType(w);

  switch ( wkbtype ) {
    case WKB_POLYGON:
    case WKB_CURVEPOLYGON:
    case WKB_MULTIPOLYGON:
      shape->type = MS_SHAPE_POLYGON;
      break;
    case WKB_LINESTRING:
    case WKB_CIRCULARSTRING:
    case WKB_COMPOUNDCURVE:
    case WKB_MULTICURVE:
    case WKB_MULTILINESTRING:
      shape->type = MS_SHAPE_LINE;
      break;
    case WKB_POINT:
    case WKB_MULTIPOINT:
      shape->type = MS_SHAPE_POINT;
      break;
    default:
      return MS_FAILURE;
  }

  return wkbConvGeometryToShape(w, shape);
}
