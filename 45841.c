msPostGISRetrievePgVersion(PGconn *pgconn)
{
#ifndef POSTGIS_HAS_SERVER_VERSION
  int pgVersion = 0;
  char *strVersion = NULL;
  char *strParts[3] = { NULL, NULL, NULL };
  int i = 0, j = 0, len = 0;
  int factor = 10000;

  if (pgconn == NULL) {
    msSetError(MS_QUERYERR, "Layer does not have a postgis connection.", "msPostGISRetrievePgVersion()");
    return MS_FAILURE;
  }

  if (! PQparameterStatus(pgconn, "server_version") )
    return MS_FAILURE;

  strVersion = msStrdup(PQparameterStatus(pgconn, "server_version"));
  if( ! strVersion )
    return MS_FAILURE;

  strParts[j] = strVersion;
  j++;
  len = strlen(strVersion);
  for( i = 0; i < len; i++ ) {
    if( strVersion[i] == '.' ) {
      strVersion[i] = '\0';

      if( j < 3 ) {
        strParts[j] = strVersion + i + 1;
        j++;
      } else {
        free(strVersion);
        msSetError(MS_QUERYERR, "Too many parts in version string.", "msPostGISRetrievePgVersion()");
        return MS_FAILURE;
      }
    }
  }

  for( j = 0; j < 3 && strParts[j]; j++ ) {
    pgVersion += factor * atoi(strParts[j]);
    factor = factor / 100;
  }
  free(strVersion);
  return pgVersion;
#else
  return PQserverVersion(pgconn);
#endif
}
