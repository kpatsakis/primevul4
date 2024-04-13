GF_Err gf_isom_insert_moov(GF_ISOFile *file)
{
	GF_MovieHeaderBox *mvhd;
	if (file->moov) return GF_OK;

	//OK, create our boxes (mvhd, iods, ...)
	file->moov = (GF_MovieBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MOOV);
	if (!file->moov) return GF_OUT_OF_MEM;
	file->moov->mov = file;
	//Header SetUp
	mvhd = (GF_MovieHeaderBox *) gf_isom_box_new_parent(&file->moov->child_boxes, GF_ISOM_BOX_TYPE_MVHD);
	if (!mvhd) return GF_OUT_OF_MEM;

	if (gf_sys_is_test_mode() ) {
		mvhd->creationTime = mvhd->modificationTime = 0;
	} else {
		u64 now = gf_isom_get_mp4time();
		mvhd->creationTime = now;
		if (!file->keep_utc)
			mvhd->modificationTime = now;
	}

	mvhd->nextTrackID = 1;
	//600 is our default movie TimeScale
	mvhd->timeScale = 600;

	file->interleavingTime = mvhd->timeScale;
	moov_on_child_box((GF_Box*)file->moov, (GF_Box *)mvhd, GF_FALSE);
	gf_list_add(file->TopBoxes, file->moov);
	return GF_OK;
}