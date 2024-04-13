int msPostGISGetPaging(layerObj *layer)
{
#ifdef USE_POSTGIS
  msPostGISLayerInfo *layerinfo = NULL;

  if (layer->debug) {
    msDebug("msPostGISGetPaging called.\n");
  }

  if(!msPostGISLayerIsOpen(layer))
    return MS_TRUE;

  assert( layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo *)layer->layerinfo;
  return layerinfo->paging;
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISEnablePaging()");
  return MS_FAILURE;
#endif
}
