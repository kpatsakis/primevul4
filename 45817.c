char *msPostGISBuildSQLBox(layerObj *layer, rectObj *rect, char *strSRID)
{

  char *strBox = NULL;
  size_t sz;

  if (layer->debug) {
    msDebug("msPostGISBuildSQLBox called.\n");
  }

  if ( strSRID ) {
    static char *strBoxTemplate = "ST_GeomFromText('POLYGON((%.15g %.15g,%.15g %.15g,%.15g %.15g,%.15g %.15g,%.15g %.15g))',%s)";
    /* 10 doubles + 1 integer + template characters */
    sz = 10 * 22 + strlen(strSRID) + strlen(strBoxTemplate);
    strBox = (char*)msSmallMalloc(sz+1); /* add space for terminating NULL */
    if ( sz <= snprintf(strBox, sz, strBoxTemplate,
                        rect->minx, rect->miny,
                        rect->minx, rect->maxy,
                        rect->maxx, rect->maxy,
                        rect->maxx, rect->miny,
                        rect->minx, rect->miny,
                        strSRID)) {
      msSetError(MS_MISCERR,"Bounding box digits truncated.","msPostGISBuildSQLBox");
      return NULL;
    }
  } else {
    static char *strBoxTemplate = "ST_GeomFromText('POLYGON((%.15g %.15g,%.15g %.15g,%.15g %.15g,%.15g %.15g,%.15g %.15g))')";
    /* 10 doubles + template characters */
    sz = 10 * 22 + strlen(strBoxTemplate);
    strBox = (char*)msSmallMalloc(sz+1); /* add space for terminating NULL */
    if ( sz <= snprintf(strBox, sz, strBoxTemplate,
                        rect->minx, rect->miny,
                        rect->minx, rect->maxy,
                        rect->maxx, rect->maxy,
                        rect->maxx, rect->miny,
                        rect->minx, rect->miny) ) {
      msSetError(MS_MISCERR,"Bounding box digits truncated.","msPostGISBuildSQLBox");
      return NULL;
    }
  }

  return strBox;

}
