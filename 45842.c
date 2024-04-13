msPostGISRetrieveVersion(PGconn *pgconn)
{
  static char* sql = "SELECT postgis_version()";
  int version = 0;
  size_t strSize;
  char *strVersion = NULL;
  char *ptr;
  char *strParts[3] = { NULL, NULL, NULL };
  int i = 0, j = 0;
  int factor = 10000;
  PGresult *pgresult = NULL;

  if ( ! pgconn ) {
    msSetError(MS_QUERYERR, "No open connection.", "msPostGISRetrieveVersion()");
    return MS_FAILURE;
  }

  pgresult = PQexecParams(pgconn, sql,0, NULL, NULL, NULL, NULL, 0);

  if ( !pgresult || PQresultStatus(pgresult) != PGRES_TUPLES_OK) {
    msSetError(MS_QUERYERR, "Error executing SQL: %s", "msPostGISRetrieveVersion()", sql);
    return MS_FAILURE;
  }

  if (PQgetisnull(pgresult, 0, 0)) {
    PQclear(pgresult);
    msSetError(MS_QUERYERR,"Null result returned.","msPostGISRetrieveVersion()");
    return MS_FAILURE;
  }

  strSize = PQgetlength(pgresult, 0, 0) + 1;
  strVersion = (char*)msSmallMalloc(strSize);
  strlcpy(strVersion, PQgetvalue(pgresult, 0, 0), strSize);
  PQclear(pgresult);

  ptr = strVersion;
  strParts[j++] = strVersion;
  while( ptr != '\0' && j < 3 ) {
    if ( *ptr == '.' ) {
      *ptr = '\0';
      strParts[j++] = ptr + 1;
    }
    if ( *ptr == ' ' ) {
      *ptr = '\0';
      break;
    }
    ptr++;
  }

  for( i = 0; i < j; i++ ) {
    version += factor * atoi(strParts[i]);
    factor = factor / 100;
  }
  free(strVersion);

  return version;
}
