char *msPostGISEscapeSQLParam(layerObj *layer, const char *pszString)
{
#ifdef USE_POSTGIS
  msPostGISLayerInfo *layerinfo = NULL;
  int nError;
  size_t nSrcLen;
  char* pszEscapedStr =NULL;

  if (layer && pszString && strlen(pszString) > 0) {
    if(!msPostGISLayerIsOpen(layer))
      msPostGISLayerOpen(layer);

    assert(layer->layerinfo != NULL);

    layerinfo = (msPostGISLayerInfo *) layer->layerinfo;
    nSrcLen = strlen(pszString);
    pszEscapedStr = (char*) msSmallMalloc( 2 * nSrcLen + 1);
    PQescapeStringConn (layerinfo->pgconn, pszEscapedStr, pszString, nSrcLen, &nError);
    if (nError != 0) {
      free(pszEscapedStr);
      pszEscapedStr = NULL;
    }
  }
  return pszEscapedStr;
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISEscapeSQLParam()");
  return NULL;
#endif
}
