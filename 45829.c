int msPostGISLayerGetShape(layerObj *layer, shapeObj *shape, resultObj *record)
{
#ifdef USE_POSTGIS

  PGresult *pgresult = NULL;
  msPostGISLayerInfo *layerinfo = NULL;

  long shapeindex = record->shapeindex;
  int resultindex = record->resultindex;

  assert(layer != NULL);
  assert(layer->layerinfo != NULL);

  if (layer->debug) {
    msDebug("msPostGISLayerGetShape called for record = %i\n", resultindex);
  }

  /* If resultindex is set, fetch the shape from the resultcache, otherwise fetch it from the DB  */
  if (resultindex >= 0) {
    int status;

    layerinfo = (msPostGISLayerInfo*) layer->layerinfo;

    /* Check the validity of the open result. */
    pgresult = layerinfo->pgresult;
    if ( ! pgresult ) {
      msSetError( MS_MISCERR,
                  "PostgreSQL result set is null.",
                  "msPostGISLayerGetShape()");
      return MS_FAILURE;
    }
    status = PQresultStatus(pgresult);
    if ( layer->debug > 1 ) {
      msDebug("msPostGISLayerGetShape query status: %s (%d)\n", PQresStatus(status), status);
    }
    if( ! ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) ) {
      msSetError( MS_MISCERR,
                  "PostgreSQL result set is not ready.",
                  "msPostGISLayerGetShape()");
      return MS_FAILURE;
    }

    /* Check the validity of the requested record number. */
    if( resultindex >= PQntuples(pgresult) ) {
      msDebug("msPostGISLayerGetShape got request for (%d) but only has %d tuples.\n", resultindex, PQntuples(pgresult));
      msSetError( MS_MISCERR,
                  "Got request larger than result set.",
                  "msPostGISLayerGetShape()");
      return MS_FAILURE;
    }

    layerinfo->rownum = resultindex; /* Only return one result. */

    /* We don't know the shape type until we read the geometry. */
    shape->type = MS_SHAPE_NULL;

    /* Return the shape, cursor access mode. */
    msPostGISReadShape(layer, shape);

    return (shape->type == MS_SHAPE_NULL) ? MS_FAILURE : MS_SUCCESS;
  } else { /* no resultindex, fetch the shape from the DB */
    int num_tuples;
    char *strSQL = 0;

    /* Fill out layerinfo with our current DATA state. */
    if ( msPostGISParseData(layer) != MS_SUCCESS) {
      return MS_FAILURE;
    }

    /*
    ** This comes *after* parsedata, because parsedata fills in
    ** layer->layerinfo.
    */
    layerinfo = (msPostGISLayerInfo*) layer->layerinfo;

    /* Build a SQL query based on our current state. */
    strSQL = msPostGISBuildSQL(layer, 0, &shapeindex);
    if ( ! strSQL ) {
      msSetError(MS_QUERYERR, "Failed to build query SQL.", "msPostGISLayerGetShape()");
      return MS_FAILURE;
    }

    if (layer->debug) {
      msDebug("msPostGISLayerGetShape query: %s\n", strSQL);
    }

    pgresult = PQexecParams(layerinfo->pgconn, strSQL,0, NULL, NULL, NULL, NULL, 0);

    /* Something went wrong. */
    if ( (!pgresult) || (PQresultStatus(pgresult) != PGRES_TUPLES_OK) ) {
      if ( layer->debug ) {
        msDebug("Error (%s) executing SQL: %s", "msPostGISLayerGetShape()\n", PQerrorMessage(layerinfo->pgconn), strSQL );
      }
      msSetError(MS_QUERYERR, "Error executing SQL: %s", "msPostGISLayerGetShape()", PQerrorMessage(layerinfo->pgconn));

      if (pgresult) {
        PQclear(pgresult);
      }
      free(strSQL);

      return MS_FAILURE;
    }

    /* Clean any existing pgresult before storing current one. */
    if(layerinfo->pgresult) PQclear(layerinfo->pgresult);
    layerinfo->pgresult = pgresult;

    /* Clean any existing SQL before storing current. */
    if(layerinfo->sql) free(layerinfo->sql);
    layerinfo->sql = strSQL;

    layerinfo->rownum = 0; /* Only return one result. */

    /* We don't know the shape type until we read the geometry. */
    shape->type = MS_SHAPE_NULL;

    num_tuples = PQntuples(pgresult);
    if (layer->debug) {
      msDebug("msPostGISLayerGetShape number of records: %d\n", num_tuples);
    }

    if (num_tuples > 0) {
      /* Get shape in random access mode. */
      msPostGISReadShape(layer, shape);
    }

    return (shape->type == MS_SHAPE_NULL) ? MS_FAILURE : ( (num_tuples > 0) ? MS_SUCCESS : MS_DONE );
  }
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISLayerGetShape()");
  return MS_FAILURE;
#endif
}
