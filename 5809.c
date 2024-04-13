static void init_reader(ISOMChannel *ch)
{
	u32 sample_desc_index=0;

	ch->au_seq_num = 1;

	assert(ch->sample==NULL);
	if (!ch->static_sample) {
		ch->static_sample = gf_isom_sample_new();
	}

	if (ch->streamType==GF_STREAM_OCR) {
		assert(!ch->sample);
		ch->sample = gf_isom_sample_new();
		ch->sample->IsRAP = RAP;
		ch->sample->DTS = ch->start;
		ch->last_state=GF_OK;
	} else if (ch->sample_num) {
		ch->sample = gf_isom_get_sample_ex(ch->owner->mov, ch->track, ch->sample_num, &sample_desc_index, ch->static_sample, &ch->sample_data_offset);
		ch->disable_seek = GF_TRUE;
		ch->au_seq_num = ch->sample_num;
	} else {
		//if seek is disabled, get the next closest sample for this time; otherwise, get the previous RAP sample for this time
		u32 mode = ch->disable_seek ? GF_ISOM_SEARCH_BACKWARD : GF_ISOM_SEARCH_SYNC_BACKWARD;

		/*take care of seeking out of the track range*/
		if (!ch->owner->frag_type && (ch->duration<=ch->start)) {
			ch->last_state = gf_isom_get_sample_for_movie_time(ch->owner->mov, ch->track, ch->duration-1, &sample_desc_index, mode, &ch->static_sample, &ch->sample_num, &ch->sample_data_offset);
		} else if (ch->start || ch->has_edit_list) {
			ch->last_state = gf_isom_get_sample_for_movie_time(ch->owner->mov, ch->track, ch->start, &sample_desc_index, mode, &ch->static_sample, &ch->sample_num, &ch->sample_data_offset);
		} else {
			ch->sample_num = 1;
			if (ch->owner->nodata) {
				ch->sample = gf_isom_get_sample_info_ex(ch->owner->mov, ch->track, ch->sample_num, &sample_desc_index, &ch->sample_data_offset, ch->static_sample);
			} else {
				ch->sample = gf_isom_get_sample_ex(ch->owner->mov, ch->track, ch->sample_num, &sample_desc_index, ch->static_sample, &ch->sample_data_offset);
			}
			if (!ch->sample) ch->last_state = GF_EOS;
		}
		if (ch->last_state) {
			ch->sample = NULL;
			ch->last_state = GF_OK;
		} else {
			ch->sample = ch->static_sample;
		}

		if (ch->has_rap && ch->has_edit_list) {
			ch->edit_sync_frame = ch->sample_num;
		}

		if (ch->sample && !ch->sample->data && ch->owner->frag_type && !ch->has_edit_list) {
			ch->sample = NULL;
			ch->sample_num = 1;
			ch->sample = gf_isom_get_sample_ex(ch->owner->mov, ch->track, ch->sample_num, &sample_desc_index, ch->static_sample, &ch->sample_data_offset);
		}
	}


	/*no sample means we're not in the track range - stop*/
	if (!ch->sample) {
		/*incomplete file - check if we're still downloading or not*/
		if (gf_isom_get_missing_bytes(ch->owner->mov, ch->track)) {
			if (!ch->owner->input_loaded) {
				ch->last_state = GF_OK;
				return;
			}
			ch->last_state = GF_ISOM_INCOMPLETE_FILE;
		} else if (ch->sample_num) {
			ch->last_state = (ch->owner->frag_type==1) ? GF_OK : GF_EOS;
			ch->to_init = 0;
		}
		return;
	}

	ch->sample_time = ch->sample->DTS;

	ch->to_init = GF_FALSE;

	ch->seek_flag = 0;
	if (ch->disable_seek) {
		ch->dts = ch->sample->DTS;
		ch->cts = ch->sample->DTS + ch->sample->CTS_Offset;
		ch->start = 0;
	} else {
		s64 cts;
		ch->dts = ch->start;
		ch->cts = ch->start;

		cts = ch->sample->DTS + ch->sample->CTS_Offset;
		if (ch->ts_offset<0)
			cts += ch->ts_offset;

		//TODO - we need to notify scene decoder how many secs elapsed between RAP and seek point
		if (ch->cts != cts) {
			ch->seek_flag = 1;
		}
	}
	if (!sample_desc_index) sample_desc_index = 1;
	ch->last_sample_desc_index = sample_desc_index;
	ch->owner->no_order_check = ch->speed < 0 ? GF_TRUE : GF_FALSE;
}