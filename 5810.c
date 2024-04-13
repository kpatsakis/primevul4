static void isoffin_push_buffer(GF_Filter *filter, ISOMReader *read, const u8 *pck_data, u32 data_size)
{
	u64 bytes_missing;
	GF_Err e;

	if (!read->mem_url) {
		char szPath[200];
		sprintf(szPath, "gmem://%p", &read->mem_blob);
		read->mem_url = gf_strdup(szPath);
	}
	read->mem_blob.data = gf_realloc(read->mem_blob.data, read->mem_blob.size + data_size);
	memcpy(read->mem_blob.data + read->mem_blob.size, pck_data, data_size);
	read->mem_blob.size += data_size;

	if (read->mem_load_mode==1) {
		u32 box_type;
		e = gf_isom_open_progressive_ex(read->mem_url, 0, 0, GF_FALSE, &read->mov, &bytes_missing, &box_type);

		if (e && (e != GF_ISOM_INCOMPLETE_FILE)) {
			gf_filter_setup_failure(filter, e);
			read->mem_load_mode = 0;
			read->in_error = e;
			return;
		}
		if (!read->mov) {
			switch (box_type) {
			case GF_4CC('m','d','a','t'):
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[IsoMedia] non fragmented ISOBMFF with moof after mdat and no underlying file cache (pipe or other stream input), not supported !\n"));
				gf_filter_setup_failure(filter, GF_NOT_SUPPORTED);
				read->mem_load_mode = 0;
				read->in_error = GF_NOT_SUPPORTED;
				break;
			default:
				read->moov_not_loaded = 1;
				break;
			}
			return;
		}

		read->frag_type = gf_isom_is_fragmented(read->mov) ? 1 : 0;
		read->timescale = gf_isom_get_timescale(read->mov);
		isor_declare_objects(read);
		read->mem_load_mode = 2;
		read->moov_not_loaded = 0;
		return;
	}
	//refresh file
	gf_isom_refresh_fragmented(read->mov, &bytes_missing, read->mem_url);

	if ((read->mem_load_mode==2) && bytes_missing)
		read->force_fetch = GF_TRUE;

}