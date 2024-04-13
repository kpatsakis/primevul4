GF_Err gf_isom_parse_movie_boxes(GF_ISOFile *mov, u32 *boxType, u64 *bytesMissing, Bool progressive_mode)
{
	GF_Err e;
	GF_Blob *blob = NULL;

	//if associated file is a blob, lock blob before parsing !
	if (mov->movieFileMap && ((mov->movieFileMap->type == GF_ISOM_DATA_MEM) || (mov->movieFileMap->type == GF_ISOM_DATA_FILE))) {
		blob = ((GF_FileDataMap *)mov->movieFileMap)->blob;
	}

	if (blob)
		gf_mx_p(blob->mx);

	unused_bytes = 0;
	e = gf_isom_parse_movie_boxes_internal(mov, boxType, bytesMissing, progressive_mode);

	if (blob)
		gf_mx_v(blob->mx);
	return e;

}