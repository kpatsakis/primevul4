int msPostGISLayerGetItems(layerObj *layer)
{
#ifdef USE_POSTGIS
  msPostGISLayerInfo *layerinfo = NULL;
  static char *strSQLTemplate = "select * from %s where false limit 0";
  PGresult *pgresult = NULL;
  char *col = NULL;
  char *sql = NULL;
  char *strFrom = NULL;
  char found_geom = 0;
  const char *value;
  int t, item_num;
  rectObj rect;

  /* A useless rectangle for our useless query */
  rect.minx = rect.miny = rect.maxx = rect.maxy = 0.0;

  assert(layer != NULL);
  assert(layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo*) layer->layerinfo;

  assert(layerinfo->pgconn);

  if (layer->debug) {
    msDebug("msPostGISLayerGetItems called.\n");
  }

  /* Fill out layerinfo with our current DATA state. */
  if ( msPostGISParseData(layer) != MS_SUCCESS) {
    return MS_FAILURE;
  }

  layerinfo = (msPostGISLayerInfo*) layer->layerinfo;

  /* This allocates a fresh string, so remember to free it... */
  strFrom = msPostGISReplaceBoxToken(layer, &rect, layerinfo->fromsource);

  /*
  ** Both the "table" and "(select ...) as sub" cases can be handled with the
  ** same SQL.
  */
  sql = (char*) msSmallMalloc(strlen(strSQLTemplate) + strlen(strFrom));
  sprintf(sql, strSQLTemplate, strFrom);
  free(strFrom);

  if (layer->debug) {
    msDebug("msPostGISLayerGetItems executing SQL: %s\n", sql);
  }

  pgresult = PQexecParams(layerinfo->pgconn, sql,0, NULL, NULL, NULL, NULL, 0);

  if ( (!pgresult) || (PQresultStatus(pgresult) != PGRES_TUPLES_OK) ) {
    if ( layer->debug ) {
      msDebug("Error (%s) executing SQL: %s", "msPostGISLayerGetItems()\n", PQerrorMessage(layerinfo->pgconn), sql);
    }
    msSetError(MS_QUERYERR, "Error executing SQL: %s", "msPostGISLayerGetItems()", PQerrorMessage(layerinfo->pgconn));
    if (pgresult) {
      PQclear(pgresult);
    }
    free(sql);
    return MS_FAILURE;
  }

  free(sql);

  layer->numitems = PQnfields(pgresult) - 1; /* dont include the geometry column (last entry)*/
  layer->items = msSmallMalloc(sizeof(char*) * (layer->numitems + 1)); /* +1 in case there is a problem finding geometry column */

  found_geom = 0; /* havent found the geom field */
  item_num = 0;

  for (t = 0; t < PQnfields(pgresult); t++) {
    col = PQfname(pgresult, t);
    if ( strcmp(col, layerinfo->geomcolumn) != 0 ) {
      /* this isnt the geometry column */
      layer->items[item_num] = msStrdup(col);
      item_num++;
    } else {
      found_geom = 1;
    }
  }

  /*
  ** consider populating the field definitions in metadata.
  */
  if((value = msOWSLookupMetadata(&(layer->metadata), "G", "types")) != NULL
      && strcasecmp(value,"auto") == 0 )
    msPostGISPassThroughFieldDefinitions( layer, pgresult );

  /*
  ** Cleanup
  */
  PQclear(pgresult);

  if (!found_geom) {
    msSetError(MS_QUERYERR, "Tried to find the geometry column in the database, but couldn't find it.  Is it mis-capitalized? '%s'", "msPostGISLayerGetItems()", layerinfo->geomcolumn);
    return MS_FAILURE;
  }

  return msPostGISLayerInitItemInfo(layer);
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISLayerGetItems()");
  return MS_FAILURE;
#endif
}
