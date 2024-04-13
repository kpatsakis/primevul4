msPostGISPassThroughFieldDefinitions( layerObj *layer,
                                      PGresult *pgresult )

{
  int i, numitems = PQnfields(pgresult);
  msPostGISLayerInfo *layerinfo = layer->layerinfo;

  for(i=0; i<numitems; i++) {
    int oid, fmod;
    const char *gml_type = "Character";
    const char *item = PQfname(pgresult,i);
    char md_item_name[256];
    char gml_width[32], gml_precision[32];

    gml_width[0] = '\0';
    gml_precision[0] = '\0';

    /* skip geometry column */
    if( strcmp(item, layerinfo->geomcolumn) == 0 )
      continue;

    oid = PQftype(pgresult,i);
    fmod = PQfmod(pgresult,i);

    if( (oid == BPCHAROID || oid == VARCHAROID) && fmod >= 4 ) {
      sprintf( gml_width, "%d", fmod-4 );

    } else if( oid == BOOLOID ) {
      gml_type = "Integer";
      sprintf( gml_width, "%d", 1 );

    } else if( oid == INT2OID ) {
      gml_type = "Integer";
      sprintf( gml_width, "%d", 5 );

    } else if( oid == INT4OID || oid == INT8OID ) {
      gml_type = "Integer";

    } else if( oid == FLOAT4OID || oid == FLOAT8OID ) {
      gml_type = "Real";

    } else if( oid == NUMERICOID ) {
      gml_type = "Real";

      if( fmod >= 4 && ((fmod - 4) & 0xFFFF) == 0 ) {
        gml_type = "Integer";
        sprintf( gml_width, "%d", (fmod - 4) >> 16 );
      } else if( fmod >= 4 ) {
        sprintf( gml_width, "%d", (fmod - 4) >> 16 );
        sprintf( gml_precision, "%d", ((fmod-4) & 0xFFFF) );
      }
    } else if( oid == DATEOID
               || oid == TIMESTAMPOID || oid == TIMESTAMPTZOID ) {
      gml_type = "Date";
    }

    snprintf( md_item_name, sizeof(md_item_name), "gml_%s_type", item );
    if( msOWSLookupMetadata(&(layer->metadata), "G", "type") == NULL )
      msInsertHashTable(&(layer->metadata), md_item_name, gml_type );

    snprintf( md_item_name, sizeof(md_item_name), "gml_%s_width", item );
    if( strlen(gml_width) > 0
        && msOWSLookupMetadata(&(layer->metadata), "G", "width") == NULL )
      msInsertHashTable(&(layer->metadata), md_item_name, gml_width );

    snprintf( md_item_name, sizeof(md_item_name), "gml_%s_precision",item );
    if( strlen(gml_precision) > 0
        && msOWSLookupMetadata(&(layer->metadata), "G", "precision")==NULL )
      msInsertHashTable(&(layer->metadata), md_item_name, gml_precision );
  }
}
