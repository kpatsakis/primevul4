void msPostGISEnablePaging(layerObj *layer, int value)
{
#ifdef USE_POSTGIS
  msPostGISLayerInfo *layerinfo = NULL;

  if (layer->debug) {
    msDebug("msPostGISEnablePaging called.\n");
  }

  if(!msPostGISLayerIsOpen(layer))
    msPostGISLayerOpen(layer);

  assert( layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo *)layer->layerinfo;
  layerinfo->paging = value;

#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISEnablePaging()");
#endif
  return;
}
