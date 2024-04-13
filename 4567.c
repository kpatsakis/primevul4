void gf_isom_delete_movie(GF_ISOFile *mov)
{
	if (!mov) return;

	//these are our two main files
	if (mov->movieFileMap) gf_isom_datamap_del(mov->movieFileMap);

#ifndef GPAC_DISABLE_ISOM_WRITE
	if (mov->editFileMap) {
		gf_isom_datamap_del(mov->editFileMap);
	}
	if (mov->finalName) gf_free(mov->finalName);
#endif

	gf_isom_box_array_del(mov->TopBoxes);
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	gf_isom_box_array_del(mov->moof_list);
	if (mov->mfra)
		gf_isom_box_del((GF_Box*)mov->mfra);
	if (mov->sidx_pts_store)
		gf_free(mov->sidx_pts_store);
	if (mov->sidx_pts_next_store)
		gf_free(mov->sidx_pts_next_store);

	if (mov->main_sidx)
		gf_isom_box_del((GF_Box*)mov->main_sidx);

	if (mov->block_buffer)
		gf_free(mov->block_buffer);
#endif
	if (mov->last_producer_ref_time)
		gf_isom_box_del((GF_Box *) mov->last_producer_ref_time);
	if (mov->fileName) gf_free(mov->fileName);
	gf_free(mov);
}