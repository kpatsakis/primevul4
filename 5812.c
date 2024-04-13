u32 isoffin_channel_switch_quality(ISOMChannel *ch, GF_ISOFile *the_file, Bool switch_up)
{
	u32 i, count, next_track, trackID, cur_track;
	s32 ref_count;

	cur_track = ch->next_track ? ch->next_track : ch->track;
	count = gf_isom_get_track_count(the_file);
	trackID = gf_isom_get_track_id(the_file, cur_track);
	next_track = 0;

	if (switch_up) {
		for (i = 0; i < count; i++) {
			ref_count = gf_isom_get_reference_count(the_file, i+1, GF_ISOM_REF_SCAL);
			if (ref_count < 0)
				return cur_track; //error
			if (ref_count == 0)
				continue;
			/*next track is the one that has the last reference of type GF_ISOM_REF_SCAL refers to this current track*/
			if ((u32)ref_count == gf_isom_has_track_reference(the_file, i+1, GF_ISOM_REF_SCAL, trackID)) {
				next_track = i+1;
				break;
			}
		}
		/*this is the highest quality*/
		if (!next_track) {
			ch->playing = GF_TRUE;
			ref_count = gf_isom_get_reference_count(the_file, ch->track, GF_ISOM_REF_BASE);
			trackID = 0;
			if (ref_count) {
				gf_isom_get_reference(the_file, ch->track, GF_ISOM_REF_BASE, 1, &trackID);
				for (i=0; i<gf_list_count(ch->owner->channels) && trackID; i++) {
					ISOMChannel *base = gf_list_get(ch->owner->channels, i);
					if (base->track_id==trackID) {
						u32 sample_desc_index;
						u64 resume_at;
						GF_Err e;
						//try to locate sync after current time in base
						resume_at = gf_timestamp_rescale(base->static_sample->DTS, base->timescale, ch->timescale);
						e = gf_isom_get_sample_for_media_time(ch->owner->mov, ch->track, resume_at, &sample_desc_index, GF_ISOM_SEARCH_SYNC_FORWARD, &ch->static_sample, &ch->sample_num, &ch->sample_data_offset);
						//found, rewind so that next fetch is the sync
						if (e==GF_OK) {
							ch->sample = NULL;
						}
						//no further sync found, realign with base timescale
						else if (e==GF_EOS) {
							e = gf_isom_get_sample_for_media_time(ch->owner->mov, ch->track, resume_at, &sample_desc_index, GF_ISOM_SEARCH_FORWARD, &ch->static_sample, &ch->sample_num, &ch->sample_data_offset);
						}
						//unknown state, realign sample num with base
						if (e<0) {
							ch->sample_num = base->sample_num;
						}
						break;
					}
				}
			}
			return cur_track;
		}
	} else {
		if (cur_track == ch->base_track)
			return cur_track;
		ref_count = gf_isom_get_reference_count(the_file, cur_track, GF_ISOM_REF_SCAL);
		if (ref_count <= 0)
			return cur_track;
		gf_isom_get_reference(the_file, cur_track, GF_ISOM_REF_SCAL, ref_count, &next_track);
		if (!next_track)
			return cur_track;

		if (ch->track != next_track) {
			ch->playing = GF_FALSE;
			ch->eos_sent = GF_TRUE;
			gf_filter_pid_set_eos(ch->pid);
		}
	}

	/*in scalable mode add SPS/PPS in-band*/
	if (ch->owner->smode)
		gf_isom_set_nalu_extract_mode(the_file, next_track, ch->nalu_extract_mode);

	return next_track;
}