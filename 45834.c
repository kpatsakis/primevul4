int msPostGISLayerOpen(layerObj *layer)
{
#ifdef USE_POSTGIS
  msPostGISLayerInfo  *layerinfo;
  int order_test = 1;

  assert(layer != NULL);

  if (layer->debug) {
    msDebug("msPostGISLayerOpen called: %s\n", layer->data);
  }

  if (layer->layerinfo) {
    if (layer->debug) {
      msDebug("msPostGISLayerOpen: Layer is already open!\n");
    }
    return MS_SUCCESS;  /* already open */
  }

  if (!layer->data) {
    msSetError(MS_QUERYERR, "Nothing specified in DATA statement.", "msPostGISLayerOpen()");
    return MS_FAILURE;
  }

  /*
  ** Initialize the layerinfo
  **/
  layerinfo = msPostGISCreateLayerInfo();

  if (((char*) &order_test)[0] == 1) {
    layerinfo->endian = LITTLE_ENDIAN;
  } else {
    layerinfo->endian = BIG_ENDIAN;
  }

  /*
  ** Get a database connection from the pool.
  */
  layerinfo->pgconn = (PGconn *) msConnPoolRequest(layer);

  /* No connection in the pool, so set one up. */
  if (!layerinfo->pgconn) {
    char *conn_decrypted;
    if (layer->debug) {
      msDebug("msPostGISLayerOpen: No connection in pool, creating a fresh one.\n");
    }

    if (!layer->connection) {
      msSetError(MS_MISCERR, "Missing CONNECTION keyword.", "msPostGISLayerOpen()");
      return MS_FAILURE;
    }

    /*
    ** Decrypt any encrypted token in connection string and attempt to connect.
    */
    conn_decrypted = msDecryptStringTokens(layer->map, layer->connection);
    if (conn_decrypted == NULL) {
      return MS_FAILURE;  /* An error should already have been produced */
    }
    layerinfo->pgconn = PQconnectdb(conn_decrypted);
    msFree(conn_decrypted);
    conn_decrypted = NULL;

    /*
    ** Connection failed, return error message with passwords ***ed out.
    */
    if (!layerinfo->pgconn || PQstatus(layerinfo->pgconn) == CONNECTION_BAD) {
      char *index, *maskeddata;
      if (layer->debug)
        msDebug("msPostGISLayerOpen: Connection failure.\n");

      maskeddata = msStrdup(layer->connection);
      index = strstr(maskeddata, "password=");
      if (index != NULL) {
        index = (char*)(index + 9);
        while (*index != '\0' && *index != ' ') {
          *index = '*';
          index++;
        }
      }

      msSetError(MS_QUERYERR, "Database connection failed (%s) with connect string '%s'\nIs the database running? Is it allowing connections? Does the specified user exist? Is the password valid? Is the database on the standard port?", "msPostGISLayerOpen()", PQerrorMessage(layerinfo->pgconn), maskeddata);

      free(maskeddata);
      free(layerinfo);
      return MS_FAILURE;
    }

    /* Register to receive notifications from the database. */
    PQsetNoticeProcessor(layerinfo->pgconn, postresqlNoticeHandler, (void *) layer);

    /* Save this connection in the pool for later. */
    msConnPoolRegister(layer, layerinfo->pgconn, msPostGISCloseConnection);
  } else {
    /* Connection in the pool should be tested to see if backend is alive. */
    if( PQstatus(layerinfo->pgconn) != CONNECTION_OK ) {
      /* Uh oh, bad connection. Can we reset it? */
      PQreset(layerinfo->pgconn);
      if( PQstatus(layerinfo->pgconn) != CONNECTION_OK ) {
        /* Nope, time to bail out. */
        msSetError(MS_QUERYERR, "PostgreSQL database connection gone bad (%s)", "msPostGISLayerOpen()", PQerrorMessage(layerinfo->pgconn));
        return MS_FAILURE;
      }

    }
  }

  /* Get the PostGIS version number from the database */
  layerinfo->version = msPostGISRetrieveVersion(layerinfo->pgconn);
  if( layerinfo->version == MS_FAILURE ) return MS_FAILURE;
  if (layer->debug)
    msDebug("msPostGISLayerOpen: Got PostGIS version %d.\n", layerinfo->version);

  /* Save the layerinfo in the layerObj. */
  layer->layerinfo = (void*)layerinfo;

  return MS_SUCCESS;
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISLayerOpen()");
  return MS_FAILURE;
#endif
}
