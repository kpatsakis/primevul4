msPostGISLayerInfo *msPostGISCreateLayerInfo(void)
{
  msPostGISLayerInfo *layerinfo = msSmallMalloc(sizeof(msPostGISLayerInfo));
  layerinfo->sql = NULL;
  layerinfo->srid = NULL;
  layerinfo->uid = NULL;
  layerinfo->pgconn = NULL;
  layerinfo->pgresult = NULL;
  layerinfo->geomcolumn = NULL;
  layerinfo->fromsource = NULL;
  layerinfo->endian = 0;
  layerinfo->rownum = 0;
  layerinfo->version = 0;
  layerinfo->paging = MS_TRUE;
  return layerinfo;
}
