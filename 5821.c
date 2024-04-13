static Bool isoffin_process_event(GF_Filter *filter, const GF_FilterEvent *evt)
{
	u32 count, i;
	Bool cancel_event = GF_TRUE;
	ISOMChannel *ch;
	ISOMReader *read = gf_filter_get_udta(filter);

	if (!read || read->disconnected) return GF_FALSE;

	if (evt->base.type == GF_FEVT_QUALITY_SWITCH) {
		count = gf_list_count(read->channels);
		for (i = 0; i < count; i++) {
			ch = (ISOMChannel *)gf_list_get(read->channels, i);
			if (ch->base_track && gf_isom_needs_layer_reconstruction(read->mov)) {
				/*ch->next_track = */ //old code, see not in isoffin_reconfigure
				isoffin_channel_switch_quality(ch, read->mov, evt->quality_switch.up);
			}
		}
		return GF_TRUE;
	}

	if (!evt->base.on_pid) return GF_FALSE;

	ch = isor_get_channel(read, evt->base.on_pid);
	if (!ch)
		return GF_FALSE;

	switch (evt->base.type) {
	case GF_FEVT_PLAY:
		isor_reset_reader(ch);
		ch->eos_sent = GF_FALSE;
		ch->speed = evt->play.speed;
		ch->initial_play_seen = GF_TRUE;
		read->reset_frag_state = 1;
		//it can happen that input_is_stop is still TRUE because we did not get called back after the stop - reset to FALSE since we now play
		read->input_is_stop = GF_FALSE;
		if (read->frag_type)
			read->frag_type = 1;

		ch->start = ch->end = 0;
		if (evt->play.speed>=0) {
			Double t;
			if (evt->play.start_range>=0) {
				t = evt->play.start_range;
				t *= ch->timescale;
				ch->start = (u64) t;
			}
			if (evt->play.end_range >= evt->play.start_range) {
				ch->end = (u64) -1;
				if (evt->play.end_range<FLT_MAX) {
					t = evt->play.end_range;
					t *= ch->timescale;
					ch->end = (u64) t;
				}
			}
		} else {
			Double end = evt->play.end_range;
			if (end==-1) end = 0;
			ch->start = (u64) (s64) (evt->play.start_range * ch->timescale);
			if (end <= evt->play.start_range)
				ch->end = (u64) (s64) (end  * ch->timescale);
		}
		ch->playing = GF_TRUE;
		ch->sample_num = evt->play.from_pck;

		ch->sap_only = evt->play.drop_non_ref ? GF_TRUE : GF_FALSE;

		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[IsoMedia] Starting channel playback "LLD" to "LLD" (%g to %g)\n", ch->start, ch->end, evt->play.start_range, evt->play.end_range));

		if (!read->nb_playing)
			gf_isom_reset_seq_num(read->mov);

		if (read->is_partial_download) read->input_loaded = GF_FALSE;

		if (evt->play.no_byterange_forward) {
			//new segment will be loaded, reset
			gf_isom_reset_tables(read->mov, GF_TRUE);
			gf_isom_reset_data_offset(read->mov, NULL);
			read->refresh_fragmented = GF_TRUE;
			read->mem_blob.size = 0;
			//send play event
			cancel_event = GF_FALSE;
		} else if (!read->nb_playing && read->pid && !read->input_loaded) {
			GF_FilterEvent fevt;
			Bool is_sidx_seek = GF_FALSE;
			u64 max_offset = GF_FILTER_NO_BO;
			count = gf_list_count(read->channels);

			//try sidx
			if (read->frag_type) {
				u32 ts;
				u64 dur=0;
				GF_Err e = gf_isom_get_file_offset_for_time(read->mov, evt->play.start_range, &max_offset);
				if (e==GF_OK) {
					if (evt->play.start_range>0)
						gf_isom_reset_tables(read->mov, GF_TRUE);

					is_sidx_seek = GF_TRUE;
					//in case we loaded moov but not sidx, update duration
					if ((gf_isom_get_sidx_duration(read->mov, &dur, &ts)==GF_OK) && dur) {
						dur = gf_timestamp_rescale(dur, ts, read->timescale);
						if (ch->duration != dur) {
							ch->duration = dur;
							gf_filter_pid_set_property(ch->pid, GF_PROP_PID_DURATION, &PROP_FRAC64_INT(ch->duration, read->timescale));
						}
					}
				}
			}

			if (!is_sidx_seek) {
				for (i=0; i< count; i++) {
					u32 mode, sample_desc_index, sample_num;
					u64 data_offset;
					GF_Err e;
					u64 time;
					ch = gf_list_get(read->channels, i);
					mode = ch->disable_seek ? GF_ISOM_SEARCH_BACKWARD : GF_ISOM_SEARCH_SYNC_BACKWARD;
					time = (u64) (evt->play.start_range * ch->timescale);

					/*take care of seeking out of the track range*/
					if (!read->frag_type && (ch->duration < time)) {
						e = gf_isom_get_sample_for_movie_time(read->mov, ch->track, ch->duration, 	&sample_desc_index, mode, NULL, &sample_num, &data_offset);
					} else {
						e = gf_isom_get_sample_for_movie_time(read->mov, ch->track, time, &sample_desc_index, mode, NULL, &sample_num, &data_offset);
					}
					if ((e == GF_OK) && (data_offset<max_offset))
						max_offset = data_offset;
				}
			}

			if ((evt->play.start_range || read->is_partial_download)  && (max_offset != GF_FILTER_NO_BO) ) {

				//send a seek request
				read->is_partial_download = GF_TRUE;
				read->wait_for_source = GF_TRUE;
				read->refresh_fragmented = GF_TRUE;

				GF_FEVT_INIT(fevt, GF_FEVT_SOURCE_SEEK, read->pid);
				fevt.seek.start_offset = max_offset;
				gf_filter_pid_send_event(read->pid, &fevt);
				gf_isom_set_byte_offset(read->mov, is_sidx_seek ? 0 : max_offset);

			}
		}
		//always request a process task upon a play
		gf_filter_post_process_task(read->filter);
		read->nb_playing++;
		//cancel event unless dash mode
		return cancel_event;

	case GF_FEVT_STOP:
 		if (read->nb_playing) read->nb_playing--;
		isor_reset_reader(ch);
		//don't send a stop if some of our channels are still waiting for initial play
		for (i=0; i<gf_list_count(read->channels); i++) {
			ISOMChannel *a_ch = gf_list_get(read->channels, i);
			if (ch==a_ch) continue;
			if (!a_ch->initial_play_seen) return GF_TRUE;
		}
		//cancel event if nothing playing
		if (read->nb_playing) return GF_TRUE;
		read->input_is_stop = GF_TRUE;
		return GF_FALSE;

	case GF_FEVT_SET_SPEED:
	case GF_FEVT_RESUME:
		ch->speed = evt->play.speed;
		if (ch->sap_only && !evt->play.drop_non_ref) {
			ch->sap_only = 2;
		} else {
			ch->sap_only = evt->play.drop_non_ref ? GF_TRUE : GF_FALSE;
		}
		//cancel event
		return GF_TRUE;
	default:
		break;
	}
	//by default don't cancel event
	return GF_FALSE;
}