static char *msPostGISReplaceBoxToken(layerObj *layer, rectObj *rect, const char *fromsource)
{
  char *result = NULL;

  if ( strstr(fromsource, BOXTOKEN) && rect ) {
    char *strBox = NULL;
    char *strSRID = NULL;

    /* We see to set the SRID on the box, but to what SRID? */
    strSRID = msPostGISBuildSQLSRID(layer);
    if ( ! strSRID ) {
      return NULL;
    }

    /* Create a suitable SQL string from the rectangle and SRID. */
    strBox = msPostGISBuildSQLBox(layer, rect, strSRID);
    if ( ! strBox ) {
      msSetError(MS_MISCERR, "Unable to build box SQL.", "msPostGISReplaceBoxToken()");
      if (strSRID) free(strSRID);
      return NULL;
    }

    /* Do the substitution. */
    while ( strstr(fromsource, BOXTOKEN) ) {
      char    *start, *end;
      char    *oldresult = result;
      size_t buffer_size = 0;
      start = strstr(fromsource, BOXTOKEN);
      end = start + BOXTOKENLENGTH;

      buffer_size = (start - fromsource) + strlen(strBox) + strlen(end) +1;
      result = (char*)msSmallMalloc(buffer_size);

      strlcpy(result, fromsource, start - fromsource +1);
      strlcpy(result + (start - fromsource), strBox, buffer_size-(start - fromsource));
      strlcat(result, end, buffer_size);

      fromsource = result;
      if (oldresult != NULL)
        free(oldresult);
    }

    if (strSRID) free(strSRID);
    if (strBox) free(strBox);
  } else {
    result = msStrdup(fromsource);
  }
  return result;

}
