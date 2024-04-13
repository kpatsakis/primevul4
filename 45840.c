msPostGISRetrievePK(layerObj *layer)
{
  PGresult *pgresult = NULL;
  char *sql = 0;
  size_t size;
  msPostGISLayerInfo *layerinfo = 0;
  int length;
  int pgVersion;
  char *pos_sep;
  char *schema = NULL;
  char *table = NULL;

  if (layer->debug) {
    msDebug("msPostGISRetrievePK called.\n");
  }

  layerinfo = (msPostGISLayerInfo *) layer->layerinfo;

  /* Attempt to separate fromsource into schema.table */
  pos_sep = strstr(layerinfo->fromsource, ".");
  if (pos_sep) {
    length = strlen(layerinfo->fromsource) - strlen(pos_sep) + 1;
    schema = (char*)msSmallMalloc(length);
    strlcpy(schema, layerinfo->fromsource, length);

    length = strlen(pos_sep);
    table = (char*)msSmallMalloc(length);
    strlcpy(table, pos_sep + 1, length);

    if (layer->debug) {
      msDebug("msPostGISRetrievePK(): Found schema %s, table %s.\n", schema, table);
    }
  }

  if (layerinfo->pgconn == NULL) {
    msSetError(MS_QUERYERR, "Layer does not have a postgis connection.", "msPostGISRetrievePK()");
    return MS_FAILURE;
  }
  pgVersion = msPostGISRetrievePgVersion(layerinfo->pgconn);

  if (pgVersion < 70000) {
    if (layer->debug) {
      msDebug("msPostGISRetrievePK(): Major version below 7.\n");
    }
    return MS_FAILURE;
  }
  if (pgVersion < 70200) {
    if (layer->debug) {
      msDebug("msPostGISRetrievePK(): Version below 7.2.\n");
    }
    return MS_FAILURE;
  }
  if (pgVersion < 70300) {
    /*
    ** PostgreSQL v7.2 has a different representation of primary keys that
    ** later versions.  This currently does not explicitly exclude
    ** multicolumn primary keys.
    */
    static char *v72sql = "select b.attname from pg_class as a, pg_attribute as b, (select oid from pg_class where relname = '%s') as c, pg_index as d where d.indexrelid = a.oid and d.indrelid = c.oid and d.indisprimary and b.attrelid = a.oid and a.relnatts = 1";
    sql = msSmallMalloc(strlen(layerinfo->fromsource) + strlen(v72sql));
    sprintf(sql, v72sql, layerinfo->fromsource);
  } else {
    /*
    ** PostgreSQL v7.3 and later treat primary keys as constraints.
    ** We only support single column primary keys, so multicolumn
    ** pks are explicitly excluded from the query.
    */
    if (schema && table) {
      static char *v73sql = "select attname from pg_attribute, pg_constraint, pg_class, pg_namespace where pg_constraint.conrelid = pg_class.oid and pg_class.oid = pg_attribute.attrelid and pg_constraint.contype = 'p' and pg_constraint.conkey[1] = pg_attribute.attnum and pg_class.relname = '%s' and pg_class.relnamespace = pg_namespace.oid and pg_namespace.nspname = '%s' and pg_constraint.conkey[2] is null";
      sql = msSmallMalloc(strlen(schema) + strlen(table) + strlen(v73sql));
      sprintf(sql, v73sql, table, schema);
      free(table);
      free(schema);
    } else {
      static char *v73sql = "select attname from pg_attribute, pg_constraint, pg_class where pg_constraint.conrelid = pg_class.oid and pg_class.oid = pg_attribute.attrelid and pg_constraint.contype = 'p' and pg_constraint.conkey[1] = pg_attribute.attnum and pg_class.relname = '%s' and pg_table_is_visible(pg_class.oid) and pg_constraint.conkey[2] is null";
      sql = msSmallMalloc(strlen(layerinfo->fromsource) + strlen(v73sql));
      sprintf(sql, v73sql, layerinfo->fromsource);
    }
  }

  if (layer->debug > 1) {
    msDebug("msPostGISRetrievePK: %s\n", sql);
  }

  layerinfo = (msPostGISLayerInfo *) layer->layerinfo;

  if (layerinfo->pgconn == NULL) {
    msSetError(MS_QUERYERR, "Layer does not have a postgis connection.", "msPostGISRetrievePK()");
    free(sql);
    return MS_FAILURE;
  }

  pgresult = PQexecParams(layerinfo->pgconn, sql, 0, NULL, NULL, NULL, NULL, 0);
  if ( !pgresult || PQresultStatus(pgresult) != PGRES_TUPLES_OK) {
    static char *tmp1 = "Error executing SQL: ";
    char *tmp2 = NULL;
    size_t size2;

    size2 = sizeof(char)*(strlen(tmp1) + strlen(sql) + 1);
    tmp2 = (char*)msSmallMalloc(size2);
    strlcpy(tmp2, tmp1, size2);
    strlcat(tmp2, sql, size2);
    msSetError(MS_QUERYERR, tmp2, "msPostGISRetrievePK()");
    free(tmp2);
    free(sql);
    return MS_FAILURE;
  }

  if (PQntuples(pgresult) < 1) {
    if (layer->debug) {
      msDebug("msPostGISRetrievePK: No results found.\n");
    }
    PQclear(pgresult);
    free(sql);
    return MS_FAILURE;
  }
  if (PQntuples(pgresult) > 1) {
    if (layer->debug) {
      msDebug("msPostGISRetrievePK: Multiple results found.\n");
    }
    PQclear(pgresult);
    free(sql);
    return MS_FAILURE;
  }

  if (PQgetisnull(pgresult, 0, 0)) {
    if (layer->debug) {
      msDebug("msPostGISRetrievePK: Null result returned.\n");
    }
    PQclear(pgresult);
    free(sql);
    return MS_FAILURE;
  }

  size = PQgetlength(pgresult, 0, 0) + 1;
  layerinfo->uid = (char*)msSmallMalloc(size);
  strlcpy(layerinfo->uid, PQgetvalue(pgresult, 0, 0), size);

  PQclear(pgresult);
  free(sql);
  return MS_SUCCESS;
}
