GF_ISOFile *gf_isom_new_movie()
{
	GF_ISOFile *mov = (GF_ISOFile*)gf_malloc(sizeof(GF_ISOFile));
	if (mov == NULL) {
		gf_isom_set_last_error(NULL, GF_OUT_OF_MEM);
		return NULL;
	}
	memset(mov, 0, sizeof(GF_ISOFile));

	/*init the boxes*/
	mov->TopBoxes = gf_list_new();
	if (!mov->TopBoxes) {
		gf_isom_set_last_error(NULL, GF_OUT_OF_MEM);
		gf_free(mov);
		return NULL;
	}

	/*default storage mode is flat*/
	mov->storageMode = GF_ISOM_STORE_FLAT;
	mov->es_id_default_sync = -1;
	return mov;
}