int msPostGISReadShape(layerObj *layer, shapeObj *shape)
{

  char *wkbstr = NULL;
  unsigned char wkbstatic[wkbstaticsize];
  unsigned char *wkb = NULL;
  wkbObj w;
  msPostGISLayerInfo *layerinfo = NULL;
  int result = 0;
  int wkbstrlen = 0;

  if (layer->debug) {
    msDebug("msPostGISReadShape called.\n");
  }

  assert(layer->layerinfo != NULL);
  layerinfo = (msPostGISLayerInfo*) layer->layerinfo;

  /* Retrieve the geometry. */
  wkbstr = (char*)PQgetvalue(layerinfo->pgresult, layerinfo->rownum, layer->numitems );
  wkbstrlen = PQgetlength(layerinfo->pgresult, layerinfo->rownum, layer->numitems);

  if ( ! wkbstr ) {
    msSetError(MS_QUERYERR, "Base64 WKB returned is null!", "msPostGISReadShape()");
    return MS_FAILURE;
  }

  if(wkbstrlen > wkbstaticsize) {
    wkb = calloc(wkbstrlen, sizeof(char));
  } else {
    wkb = wkbstatic;
  }
#if TRANSFER_ENCODING == 64
  result = msPostGISBase64Decode(wkb, wkbstr, wkbstrlen - 1);
#else
  result = msPostGISHexDecode(wkb, wkbstr, wkbstrlen);
#endif

  if( ! result ) {
    if(wkb!=wkbstatic) free(wkb);
    return MS_FAILURE;
  }

  /* Initialize our wkbObj */
  w.wkb = (char*)wkb;
  w.ptr = w.wkb;
  w.size = (wkbstrlen - 1)/2;

  /* Set the type map according to what version of PostGIS we are dealing with */
  if( layerinfo->version >= 20000 ) /* PostGIS 2.0+ */
    w.typemap = wkb_postgis20;
  else
    w.typemap = wkb_postgis15;

  switch (layer->type) {

    case MS_LAYER_POINT:
      shape->type = MS_SHAPE_POINT;
      result = wkbConvGeometryToShape(&w, shape);
      break;

    case MS_LAYER_LINE:
      shape->type = MS_SHAPE_LINE;
      result = wkbConvGeometryToShape(&w, shape);
      break;

    case MS_LAYER_POLYGON:
      shape->type = MS_SHAPE_POLYGON;
      result = wkbConvGeometryToShape(&w, shape);
      break;

    case MS_LAYER_ANNOTATION:
    case MS_LAYER_QUERY:
    case MS_LAYER_CHART:
      result = msPostGISFindBestType(&w, shape);
      break;

    case MS_LAYER_RASTER:
      msDebug("Ignoring MS_LAYER_RASTER in msPostGISReadShape.\n");
      break;

    case MS_LAYER_CIRCLE:
      msDebug("Ignoring MS_LAYER_RASTER in msPostGISReadShape.\n");
      break;

    default:
      msDebug("Unsupported layer type in msPostGISReadShape()!\n");
      break;
  }

  /* All done with WKB geometry, free it! */
  if(wkb!=wkbstatic) free(wkb);

  if (result != MS_FAILURE) {
    int t;
    long uid;
    char *tmp;
    /* Found a drawable shape, so now retreive the attributes. */

    shape->values = (char**) msSmallMalloc(sizeof(char*) * layer->numitems);
    for ( t = 0; t < layer->numitems; t++) {
      int size = PQgetlength(layerinfo->pgresult, layerinfo->rownum, t);
      char *val = (char*)PQgetvalue(layerinfo->pgresult, layerinfo->rownum, t);
      int isnull = PQgetisnull(layerinfo->pgresult, layerinfo->rownum, t);
      if ( isnull ) {
        shape->values[t] = msStrdup("");
      } else {
        shape->values[t] = (char*) msSmallMalloc(size + 1);
        memcpy(shape->values[t], val, size);
        shape->values[t][size] = '\0'; /* null terminate it */
        msStringTrimBlanks(shape->values[t]);
      }
      if( layer->debug > 4 ) {
        msDebug("msPostGISReadShape: PQgetlength = %d\n", size);
      }
      if( layer->debug > 1 ) {
        msDebug("msPostGISReadShape: [%s] \"%s\"\n", layer->items[t], shape->values[t]);
      }
    }

    /* t is the geometry, t+1 is the uid */
    tmp = PQgetvalue(layerinfo->pgresult, layerinfo->rownum, t + 1);
    if( tmp ) {
      uid = strtol( tmp, NULL, 10 );
    } else {
      uid = 0;
    }
    if( layer->debug > 4 ) {
      msDebug("msPostGISReadShape: Setting shape->index = %d\n", uid);
      msDebug("msPostGISReadShape: Setting shape->resultindex = %d\n", layerinfo->rownum);
    }
    shape->index = uid;
    shape->resultindex = layerinfo->rownum;

    if( layer->debug > 2 ) {
      msDebug("msPostGISReadShape: [index] %d\n",  shape->index);
    }

    shape->numvalues = layer->numitems;

    msComputeBounds(shape);
  }

  if( layer->debug > 2 ) {
    char *tmp = msShapeToWKT(shape);
    msDebug("msPostGISReadShape: [shape] %s\n", tmp);
    free(tmp);
  }

  return MS_SUCCESS;
}
