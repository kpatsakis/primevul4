GF_Err isoffin_configure_pid(GF_Filter *filter, GF_FilterPid *pid, Bool is_remove)
{
	const GF_PropertyValue *prop;
	ISOMReader *read = gf_filter_get_udta(filter);

	if (is_remove) {
		isoffin_disconnect(read);
		return GF_OK;
	}
	//check if we  have a file path; if not, this is a pure stream of boxes (no local file cache)
	prop = gf_filter_pid_get_property(pid, GF_PROP_PID_FILEPATH);
	if (!prop || !prop->value.string) {
		if (!read->mem_load_mode)
			read->mem_load_mode = 1;
		if (!read->pid) read->pid = pid;
		read->input_loaded = GF_FALSE;
		return GF_OK;
	}

	if (read->pid && prop->value.string) {
		const char *next_url = prop->value.string;
		u64 sr, er;
		u32 crc = gf_crc_32(next_url, (u32) strlen(next_url) );

		sr = er = 0;
		prop = gf_filter_pid_get_property(read->pid, GF_PROP_PID_FILE_RANGE);
		if (prop) {
			sr = prop->value.lfrac.num;
			er = prop->value.lfrac.den;
		}

		//if eos is signaled, don't check for crc since we might have the same blob address (same alloc)
		if (!read->eos_signaled && (read->src_crc == crc) && (read->start_range==sr) && (read->end_range==er)) {
			GF_LOG(GF_LOG_DEBUG, GF_LOG_DASH, ("[IsoMedia] same URL crc and range for %s, skipping reconfigure\n", next_url));
			return GF_OK;
		}
		read->src_crc = crc;
		read->start_range = sr;
		read->end_range = er;
		read->input_loaded = GF_FALSE;
		read->eos_signaled = GF_FALSE;
		
		//we need to reconfigure
		return isoffin_reconfigure(filter, read, next_url);
	}

	read->pid = pid;
	prop = gf_filter_pid_get_property(pid, GF_PROP_PID_FILE_CACHED);
	if (prop && prop->value.boolean) {
		GF_FilterEvent evt;
		read->input_loaded = GF_TRUE;
		GF_FEVT_INIT(evt, GF_FEVT_PLAY_HINT, pid);
		evt.play.full_file_only=1;
		gf_filter_pid_send_event(pid, &evt);
	}
	return isoffin_setup(filter, read);
}