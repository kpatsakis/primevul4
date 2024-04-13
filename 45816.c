char *msPostGISBuildSQL(layerObj *layer, rectObj *rect, long *uid)
{

  msPostGISLayerInfo *layerinfo = 0;
  char *strFrom = 0;
  char *strItems = 0;
  char *strWhere = 0;
  char *strSQL = 0;
  static char *strSQLTemplate0 = "select %s from %s where %s";
  static char *strSQLTemplate1 = "select %s from %s%s";
  char *strSQLTemplate = 0;

  if (layer->debug) {
    msDebug("msPostGISBuildSQL called.\n");
  }

  assert( layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo *)layer->layerinfo;

  strItems = msPostGISBuildSQLItems(layer);
  if ( ! strItems ) {
    msSetError(MS_MISCERR, "Failed to build SQL items.", "msPostGISBuildSQL()");
    return NULL;
  }

  strFrom = msPostGISBuildSQLFrom(layer, rect);

  if ( ! strFrom ) {
    msSetError(MS_MISCERR, "Failed to build SQL 'from'.", "msPostGISBuildSQL()");
    return NULL;
  }

  /* If there's BOX hackery going on, we don't want to append a box index test at
     the end of the query, the user is going to be responsible for making things
     work with their hackery. */
  if ( strstr(layerinfo->fromsource, BOXTOKEN) )
    strWhere = msPostGISBuildSQLWhere(layer, NULL, uid);
  else
    strWhere = msPostGISBuildSQLWhere(layer, rect, uid);

  if ( ! strWhere ) {
    msSetError(MS_MISCERR, "Failed to build SQL 'where'.", "msPostGISBuildSQL()");
    return NULL;
  }

  strSQLTemplate = strlen(strWhere) ? strSQLTemplate0 : strSQLTemplate1;

  strSQL = msSmallMalloc(strlen(strSQLTemplate) + strlen(strFrom) + strlen(strItems) + strlen(strWhere));
  sprintf(strSQL, strSQLTemplate, strItems, strFrom, strWhere);
  if (strItems) free(strItems);
  if (strFrom) free(strFrom);
  if (strWhere) free(strWhere);

  return strSQL;

}
