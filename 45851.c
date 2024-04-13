wkbConvPointToShape(wkbObj *w, shapeObj *shape)
{
  int type;
  lineObj line;

  /*endian = */wkbReadChar(w);
  type = wkbTypeMap(w,wkbReadInt(w));

  if( type != WKB_POINT ) return MS_FAILURE;

  if( ! (shape->type == MS_SHAPE_POINT) ) return MS_FAILURE;
  line.numpoints = 1;
  line.point = msSmallMalloc(sizeof(pointObj));
  line.point[0] = wkbReadPoint(w);
  msAddLineDirectly(shape, &line);
  return MS_SUCCESS;
}
