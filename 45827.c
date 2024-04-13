int msPostGISLayerClose(layerObj *layer)
{
#ifdef USE_POSTGIS

  if (layer->debug) {
    msDebug("msPostGISLayerClose called: %s\n", layer->data);
  }

  if( layer->layerinfo ) {
    msPostGISFreeLayerInfo(layer);
  }

  return MS_SUCCESS;
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISLayerClose()");
  return MS_FAILURE;
#endif
}
