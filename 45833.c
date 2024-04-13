int msPostGISLayerNextShape(layerObj *layer, shapeObj *shape)
{
#ifdef USE_POSTGIS
  msPostGISLayerInfo  *layerinfo;

  if (layer->debug) {
    msDebug("msPostGISLayerNextShape called.\n");
  }

  assert(layer != NULL);
  assert(layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo*) layer->layerinfo;

  shape->type = MS_SHAPE_NULL;

  /*
  ** Roll through pgresult until we hit non-null shape (usually right away).
  */
  while (shape->type == MS_SHAPE_NULL) {
    if (layerinfo->rownum < PQntuples(layerinfo->pgresult)) {
      /* Retrieve this shape, cursor access mode. */
      msPostGISReadShape(layer, shape);
      if( shape->type != MS_SHAPE_NULL ) {
        (layerinfo->rownum)++; /* move to next shape */
        return MS_SUCCESS;
      } else {
        (layerinfo->rownum)++; /* move to next shape */
      }
    } else {
      return MS_DONE;
    }
  }

  /* Found nothing, clean up and exit. */
  msFreeShape(shape);

  return MS_FAILURE;
#else
  msSetError( MS_MISCERR,
              "PostGIS support is not available.",
              "msPostGISLayerNextShape()");
  return MS_FAILURE;
#endif
}
