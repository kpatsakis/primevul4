void msPostGISFreeLayerInfo(layerObj *layer)
{
  msPostGISLayerInfo *layerinfo = NULL;
  layerinfo = (msPostGISLayerInfo*)layer->layerinfo;
  if ( layerinfo->sql ) free(layerinfo->sql);
  if ( layerinfo->uid ) free(layerinfo->uid);
  if ( layerinfo->srid ) free(layerinfo->srid);
  if ( layerinfo->geomcolumn ) free(layerinfo->geomcolumn);
  if ( layerinfo->fromsource ) free(layerinfo->fromsource);
  if ( layerinfo->pgresult ) PQclear(layerinfo->pgresult);
  if ( layerinfo->pgconn ) msConnPoolRelease(layer, layerinfo->pgconn);
  free(layerinfo);
  layer->layerinfo = NULL;
}
