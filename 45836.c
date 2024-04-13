int msPostGISParseData(layerObj *layer)
{
  char *pos_opt, *pos_scn, *tmp, *pos_srid, *pos_uid, *pos_geom, *data;
  int slength;
  msPostGISLayerInfo *layerinfo;

  assert(layer != NULL);
  assert(layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo*)(layer->layerinfo);

  if (layer->debug) {
    msDebug("msPostGISParseData called.\n");
  }

  if (!layer->data) {
    msSetError(MS_QUERYERR, "Missing DATA clause. DATA statement must contain 'geometry_column from table_name' or 'geometry_column from (sub-query) as sub'.", "msPostGISParseData()");
    return MS_FAILURE;
  }
  data = layer->data;

  /*
  ** Clean up any existing strings first, as we will be populating these fields.
  */
  if( layerinfo->srid ) {
    free(layerinfo->srid);
    layerinfo->srid = NULL;
  }
  if( layerinfo->uid ) {
    free(layerinfo->uid);
    layerinfo->uid = NULL;
  }
  if( layerinfo->geomcolumn ) {
    free(layerinfo->geomcolumn);
    layerinfo->geomcolumn = NULL;
  }
  if( layerinfo->fromsource ) {
    free(layerinfo->fromsource);
    layerinfo->fromsource = NULL;
  }

  /*
  ** Look for the optional ' using unique ID' string first.
  */
  pos_uid = strcasestr(data, " using unique ");
  if (pos_uid) {
    /* Find the end of this case 'using unique ftab_id using srid=33' */
    tmp = strstr(pos_uid + 14, " ");
    /* Find the end of this case 'using srid=33 using unique ftab_id' */
    if (!tmp) {
      tmp = pos_uid + strlen(pos_uid);
    }
    layerinfo->uid = (char*) msSmallMalloc((tmp - (pos_uid + 14)) + 1);
    strlcpy(layerinfo->uid, pos_uid + 14, tmp - (pos_uid + 14)+1);
    msStringTrim(layerinfo->uid);
  }

  /*
  ** Look for the optional ' using srid=333 ' string next.
  */
  pos_srid = strcasestr(data, " using srid=");
  if (!pos_srid) {
    layerinfo->srid = (char*) msSmallMalloc(1);
    (layerinfo->srid)[0] = '\0'; /* no SRID, so return just null terminator*/
  } else {
    slength = strspn(pos_srid + 12, "-0123456789");
    if (!slength) {
      msSetError(MS_QUERYERR, "Error parsing PostGIS DATA variable. You specified 'USING SRID' but didnt have any numbers! %s", "msPostGISParseData()", data);
      return MS_FAILURE;
    } else {
      layerinfo->srid = (char*) msSmallMalloc(slength + 1);
      strlcpy(layerinfo->srid, pos_srid + 12, slength+1);
      msStringTrim(layerinfo->srid);
    }
  }

  /*
  ** This is a little hack so the rest of the code works.
  ** pos_opt should point to the start of the optional blocks.
  **
  ** If they are both set, return the smaller one.
  */
  if (pos_srid && pos_uid) {
    pos_opt = (pos_srid > pos_uid) ? pos_uid : pos_srid;
  }
  /* If one or none is set, return the larger one. */
  else {
    pos_opt = (pos_srid > pos_uid) ? pos_srid : pos_uid;
  }
  /* No pos_opt? Move it to the end of the string. */
  if (!pos_opt) {
    pos_opt = data + strlen(data);
  }

  /*
  ** Scan for the 'geometry from table' or 'geometry from () as foo' clause.
  */

  /* Find the first non-white character to start from */
  pos_geom = data;
  while( *pos_geom == ' ' || *pos_geom == '\t' || *pos_geom == '\n' || *pos_geom == '\r' )
    pos_geom++;

  /* Find the end of the geom column name */
  pos_scn = strcasestr(data, " from ");
  if (!pos_scn) {
    msSetError(MS_QUERYERR, "Error parsing PostGIS DATA variable. Must contain 'geometry from table' or 'geometry from (subselect) as foo'. %s", "msPostGISParseData()", data);
    return MS_FAILURE;
  }

  /* Copy the geometry column name */
  layerinfo->geomcolumn = (char*) msSmallMalloc((pos_scn - pos_geom) + 1);
  strlcpy(layerinfo->geomcolumn, pos_geom, pos_scn - pos_geom+1);
  msStringTrim(layerinfo->geomcolumn);

  /* Copy the table name or sub-select clause */
  layerinfo->fromsource = (char*) msSmallMalloc((pos_opt - (pos_scn + 6)) + 1);
  strlcpy(layerinfo->fromsource, pos_scn + 6, pos_opt - (pos_scn + 6)+1);
  msStringTrim(layerinfo->fromsource);

  /* Something is wrong, our goemetry column and table references are not there. */
  if (strlen(layerinfo->fromsource) < 1 || strlen(layerinfo->geomcolumn) < 1) {
    msSetError(MS_QUERYERR, "Error parsing PostGIS DATA variable.  Must contain 'geometry from table' or 'geometry from (subselect) as foo'. %s", "msPostGISParseData()", data);
    return MS_FAILURE;
  }

  /*
  ** We didn't find a ' using unique ' in the DATA string so try and find a
  ** primary key on the table.
  */
  if ( ! (layerinfo->uid) ) {
    if ( strstr(layerinfo->fromsource, " ") ) {
      msSetError(MS_QUERYERR, "Error parsing PostGIS DATA variable.  You must specify 'using unique' when supplying a subselect in the data definition.", "msPostGISParseData()");
      return MS_FAILURE;
    }
    if ( msPostGISRetrievePK(layer) != MS_SUCCESS ) {
      /* No user specified unique id so we will use the PostgreSQL oid */
      /* TODO: Deprecate this, oids are deprecated in PostgreSQL */
      layerinfo->uid = msStrdup("oid");
    }
  }

  if (layer->debug) {
    msDebug("msPostGISParseData: unique_column=%s, srid=%s, geom_column_name=%s, table_name=%s\n", layerinfo->uid, layerinfo->srid, layerinfo->geomcolumn, layerinfo->fromsource);
  }
  return MS_SUCCESS;
}
