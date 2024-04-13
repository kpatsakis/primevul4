int msPostGISLayerIsOpen(layerObj *layer)
{
#ifdef USE_POSTGIS

  if (layer->debug) {
    msDebug("msPostGISLayerIsOpen called.\n");
  }

  if (layer->layerinfo)
    return MS_TRUE;
  else
    return MS_FALSE;
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISLayerIsOpen()");
  return MS_FAILURE;
#endif
}
