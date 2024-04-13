wkbSkipGeometry(wkbObj *w)
{
  int type, npoints, nrings, ngeoms, i;
  /*endian = */wkbReadChar(w);
  type = wkbTypeMap(w,wkbReadInt(w));
  switch(type) {
    case WKB_POINT:
      w->ptr += 2 * sizeof(double);
      break;
    case WKB_CIRCULARSTRING:
    case WKB_LINESTRING:
      npoints = wkbReadInt(w);
      w->ptr += npoints * 2 * sizeof(double);
      break;
    case WKB_POLYGON:
      nrings = wkbReadInt(w);
      for ( i = 0; i < nrings; i++ ) {
        npoints = wkbReadInt(w);
        w->ptr += npoints * 2 * sizeof(double);
      }
      break;
    case WKB_MULTIPOINT:
    case WKB_MULTILINESTRING:
    case WKB_MULTIPOLYGON:
    case WKB_GEOMETRYCOLLECTION:
    case WKB_COMPOUNDCURVE:
    case WKB_CURVEPOLYGON:
    case WKB_MULTICURVE:
    case WKB_MULTISURFACE:
      ngeoms = wkbReadInt(w);
      for ( i = 0; i < ngeoms; i++ ) {
        wkbSkipGeometry(w);
      }
  }
}
