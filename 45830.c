int msPostGISLayerInitItemInfo(layerObj *layer)
{
#ifdef USE_POSTGIS
  int i;
  int *itemindexes ;

  if (layer->debug) {
    msDebug("msPostGISLayerInitItemInfo called.\n");
  }

  if (layer->numitems == 0) {
    return MS_SUCCESS;
  }

  if (layer->iteminfo) {
    free(layer->iteminfo);
  }

  layer->iteminfo = msSmallMalloc(sizeof(int) * layer->numitems);
  if (!layer->iteminfo) {
    msSetError(MS_MEMERR, "Out of memory.", "msPostGISLayerInitItemInfo()");
    return MS_FAILURE;
  }

  itemindexes = (int*)layer->iteminfo;
  for (i = 0; i < layer->numitems; i++) {
    itemindexes[i] = i; /* Last item is always the geometry. The rest are non-geometry. */
  }

  return MS_SUCCESS;
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISLayerInitItemInfo()");
  return MS_FAILURE;
#endif
}
