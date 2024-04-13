char *msPostGISBuildSQLWhere(layerObj *layer, rectObj *rect, long *uid)
{
  char *strRect = 0;
  char *strFilter = 0;
  char *strUid = 0;
  char *strWhere = 0;
  char *strLimit = 0;
  char *strOffset = 0;
  size_t strRectLength = 0;
  size_t strFilterLength = 0;
  size_t strUidLength = 0;
  size_t strLimitLength = 0;
  size_t strOffsetLength = 0;
  size_t bufferSize = 0;
  int insert_and = 0;
  msPostGISLayerInfo *layerinfo;

  if (layer->debug) {
    msDebug("msPostGISBuildSQLWhere called.\n");
  }

  assert( layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo *)layer->layerinfo;

  if ( ! layerinfo->fromsource ) {
    msSetError(MS_MISCERR, "Layerinfo->fromsource is not initialized.", "msPostGISBuildSQLWhere()");
    return NULL;
  }

  /* Populate strLimit, if necessary. */
  if ( layerinfo->paging && layer->maxfeatures >= 0 ) {
    static char *strLimitTemplate = " limit %d";
    strLimit = msSmallMalloc(strlen(strLimitTemplate) + 12);
    sprintf(strLimit, strLimitTemplate, layer->maxfeatures);
    strLimitLength = strlen(strLimit);
  }

  /* Populate strOffset, if necessary. */
  if ( layerinfo->paging && layer->startindex > 0 ) {
    static char *strOffsetTemplate = " offset %d";
    strOffset = msSmallMalloc(strlen(strOffsetTemplate) + 12);
    sprintf(strOffset, strOffsetTemplate, layer->startindex-1);
    strOffsetLength = strlen(strOffset);
  }

  /* Populate strRect, if necessary. */
  if ( rect && layerinfo->geomcolumn ) {
    char *strBox = 0;
    char *strSRID = 0;
    size_t strBoxLength = 0;
    static char *strRectTemplate = "%s && %s";

    /* We see to set the SRID on the box, but to what SRID? */
    strSRID = msPostGISBuildSQLSRID(layer);
    if ( ! strSRID ) {
      return NULL;
    }

    strBox = msPostGISBuildSQLBox(layer, rect, strSRID);
    if ( strBox ) {
      strBoxLength = strlen(strBox);
    } else {
      msSetError(MS_MISCERR, "Unable to build box SQL.", "msPostGISBuildSQLWhere()");
      return NULL;
    }

    strRect = (char*)msSmallMalloc(strlen(strRectTemplate) + strBoxLength + strlen(layerinfo->geomcolumn));
    sprintf(strRect, strRectTemplate, layerinfo->geomcolumn, strBox);
    strRectLength = strlen(strRect);
    if (strBox) free(strBox);
    if (strSRID) free(strSRID);
  }

  /* Populate strFilter, if necessary. */
  if ( layer->filter.string ) {
    static char *strFilterTemplate = "(%s)";
    strFilter = (char*)msSmallMalloc(strlen(strFilterTemplate) + strlen(layer->filter.string));
    sprintf(strFilter, strFilterTemplate, layer->filter.string);
    strFilterLength = strlen(strFilter);
  }

  /* Populate strUid, if necessary. */
  if ( uid ) {
    static char *strUidTemplate = "\"%s\" = %ld";
    strUid = (char*)msSmallMalloc(strlen(strUidTemplate) + strlen(layerinfo->uid) + 64);
    sprintf(strUid, strUidTemplate, layerinfo->uid, *uid);
    strUidLength = strlen(strUid);
  }

  bufferSize = strRectLength + 5 + strFilterLength + 5 + strUidLength
               + strLimitLength + strOffsetLength;
  strWhere = (char*)msSmallMalloc(bufferSize);
  *strWhere = '\0';
  if ( strRect ) {
    strlcat(strWhere, strRect, bufferSize);
    insert_and++;
    free(strRect);
  }
  if ( strFilter ) {
    if ( insert_and ) {
      strlcat(strWhere, " and ", bufferSize);
    }
    strlcat(strWhere, strFilter, bufferSize);
    free(strFilter);
    insert_and++;
  }
  if ( strUid ) {
    if ( insert_and ) {
      strlcat(strWhere, " and ", bufferSize);
    }
    strlcat(strWhere, strUid, bufferSize);
    free(strUid);
    insert_and++;
  }
  if ( strLimit ) {
    strlcat(strWhere, strLimit, bufferSize);
    free(strLimit);
  }
  if ( strOffset ) {
    strlcat(strWhere, strOffset, bufferSize);
    free(strOffset);
  }

  return strWhere;
}
