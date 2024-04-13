GF_Err isoffin_initialize(GF_Filter *filter)
{
	ISOMReader *read = gf_filter_get_udta(filter);
	GF_Err e = GF_OK;
	read->filter = filter;
	read->channels = gf_list_new();

	if (read->xps_check==MP4DMX_XPS_AUTO) {
		read->xps_check = (read->smode==MP4DMX_SPLIT_EXTRACTORS) ? MP4DMX_XPS_KEEP : MP4DMX_XPS_REMOVE;
	}

	if (read->src) {
		read->input_loaded = GF_TRUE;
		return isoffin_setup(filter, read);
	}
	else if (read->mov) {
		read->extern_mov = GF_TRUE;
		read->input_loaded = GF_TRUE;
		read->frag_type = gf_isom_is_fragmented(read->mov) ? 1 : 0;
		read->timescale = gf_isom_get_timescale(read->mov);

		if (read->sigfrag) {
			gf_isom_enable_traf_map_templates(read->mov);
		}

		if (read->catseg) {
			e = gf_isom_open_segment(read->mov, read->catseg, 0, 0, 0);
		}
		if (!e)
			e = isor_declare_objects(read);

		gf_filter_post_process_task(filter);
	}
	return e;
}