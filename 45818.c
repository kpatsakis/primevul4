char *msPostGISBuildSQLItems(layerObj *layer)
{

  char *strEndian = NULL;
  char *strGeom = NULL;
  char *strItems = NULL;
  msPostGISLayerInfo *layerinfo = NULL;

  if (layer->debug) {
    msDebug("msPostGISBuildSQLItems called.\n");
  }

  assert( layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo *)layer->layerinfo;

  if ( ! layerinfo->geomcolumn ) {
    msSetError(MS_MISCERR, "layerinfo->geomcolumn is not initialized.", "msPostGISBuildSQLItems()");
    return NULL;
  }

  /*
  ** Get the server to transform the geometry into our
  ** native endian before transmitting it to us..
  */
  if (layerinfo->endian == LITTLE_ENDIAN) {
    strEndian = "NDR";
  } else {
    strEndian = "XDR";
  }

  {
    /*
    ** We transfer the geometry from server to client as a
    ** hex or base64 encoded WKB byte-array. We will have to decode this
    ** data once we get it. Forcing to 2D (via the AsBinary function
    ** which includes a 2D force in it) removes ordinates we don't
    ** need, saving transfer and encode/decode time.
    */
#if TRANSFER_ENCODING == 64
    static char *strGeomTemplate = "encode(ST_AsBinary(ST_Force_2D(\"%s\"),'%s'),'base64') as geom,\"%s\"";
#else
    static char *strGeomTemplate = "encode(ST_AsBinary(ST_Force_2D(\"%s\"),'%s'),'hex') as geom,\"%s\"";
#endif
    strGeom = (char*)msSmallMalloc(strlen(strGeomTemplate) + strlen(strEndian) + strlen(layerinfo->geomcolumn) + strlen(layerinfo->uid));
    sprintf(strGeom, strGeomTemplate, layerinfo->geomcolumn, strEndian, layerinfo->uid);
  }

  if( layer->debug > 1 ) {
    msDebug("msPostGISBuildSQLItems: %d items requested.\n",layer->numitems);
  }

  /*
  ** Not requesting items? We just need geometry and unique id.
  */
  if (layer->numitems == 0) {
    strItems = msStrdup(strGeom);
  }
  /*
  ** Build SQL to pull all the items.
  */
  else {
    int length = strlen(strGeom) + 2;
    int t;
    for ( t = 0; t < layer->numitems; t++ ) {
      length += strlen(layer->items[t]) + 3; /* itemname + "", */
    }
    strItems = (char*)msSmallMalloc(length);
    strItems[0] = '\0';
    for ( t = 0; t < layer->numitems; t++ ) {
      strlcat(strItems, "\"", length);
      strlcat(strItems, layer->items[t], length);
      strlcat(strItems, "\",", length);
    }
    strlcat(strItems, strGeom, length);
  }

  free(strGeom);
  return strItems;
}
