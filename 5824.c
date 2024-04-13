ISOMChannel *isor_create_channel(ISOMReader *read, GF_FilterPid *pid, u32 track, u32 item_id, Bool force_no_extractors)
{
	ISOMChannel *ch;
	const GF_PropertyValue *p;
	s64 ts_shift;
	if (!read->mov) return NULL;

	GF_SAFEALLOC(ch, ISOMChannel);
	if (!ch) {
		return NULL;
	}
	ch->owner = read;
	ch->pid = pid;
	ch->to_init = GF_TRUE;
	gf_list_add(read->channels, ch);
	ch->track = track;
	ch->item_id = item_id;

	ch->nalu_extract_mode = 0;
	ch->track_id = gf_isom_get_track_id(read->mov, ch->track);
	switch (gf_isom_get_media_type(ch->owner->mov, ch->track)) {
	case GF_ISOM_MEDIA_OCR:
		ch->streamType = GF_STREAM_OCR;
		break;
	case GF_ISOM_MEDIA_SCENE:
		ch->streamType = GF_STREAM_SCENE;
		break;
	case GF_ISOM_MEDIA_VISUAL:
	case GF_ISOM_MEDIA_AUXV:
	case GF_ISOM_MEDIA_PICT:
		gf_isom_get_reference(ch->owner->mov, ch->track, GF_ISOM_REF_BASE, 1, &ch->base_track);
		//use base track only if avc/svc or hevc/lhvc. If avc+lhvc we need different rules
		if ( gf_isom_get_avc_svc_type(ch->owner->mov, ch->base_track, 1) == GF_ISOM_AVCTYPE_AVC_ONLY) {
			if ( gf_isom_get_hevc_lhvc_type(ch->owner->mov, ch->track, 1) >= GF_ISOM_HEVCTYPE_HEVC_ONLY) {
				ch->base_track=0;
			}
		}
		ch->next_track = 0;
		/*in scalable mode add SPS/PPS in-band*/
		if (ch->base_track)
			ch->nalu_extract_mode = GF_ISOM_NALU_EXTRACT_INBAND_PS_FLAG /*| GF_ISOM_NALU_EXTRACT_ANNEXB_FLAG*/;
		break;
	}
	if (!read->noedit) {
		ch->ts_offset = 0;
		ch->has_edit_list = gf_isom_get_edit_list_type(ch->owner->mov, ch->track, &ch->ts_offset) ? GF_TRUE : GF_FALSE;
		if (!ch->has_edit_list && ch->ts_offset) {
			//if >0 this is a hold, we signal positive delay
			//if <0 this is a skip, we signal negative delay
			gf_filter_pid_set_property(pid, GF_PROP_PID_DELAY, &PROP_LONGSINT( ch->ts_offset) );
		}
	} else
		ch->has_edit_list = GF_FALSE;

	ch->has_rap = (gf_isom_has_sync_points(ch->owner->mov, ch->track)==1) ? GF_TRUE : GF_FALSE;
	gf_filter_pid_set_property(pid, GF_PROP_PID_HAS_SYNC, &PROP_BOOL(ch->has_rap) );
	//some fragmented files do not advertize a sync sample table (legal) so we need to update as soon as we fetch a fragment
	//to see if we are all-intra (as detected here) or not
	if (!ch->has_rap && ch->owner->frag_type)
		ch->check_has_rap = GF_TRUE;
	ch->timescale = gf_isom_get_media_timescale(ch->owner->mov, ch->track);

	ts_shift = gf_isom_get_cts_to_dts_shift(ch->owner->mov, ch->track);
	if (ts_shift) {
		gf_filter_pid_set_property(pid, GF_PROP_PID_CTS_SHIFT, &PROP_UINT((u32) ts_shift) );
	}

	if (!track || !gf_isom_is_track_encrypted(read->mov, track)) {
		if (force_no_extractors) {
			ch->nalu_extract_mode = GF_ISOM_NALU_EXTRACT_LAYER_ONLY;
		} else {
			switch (read->smode) {
			case MP4DMX_SPLIT_EXTRACTORS:
				ch->nalu_extract_mode = GF_ISOM_NALU_EXTRACT_INSPECT | GF_ISOM_NALU_EXTRACT_TILE_ONLY;
				break;
			case MP4DMX_SPLIT:
				ch->nalu_extract_mode = GF_ISOM_NALU_EXTRACT_LAYER_ONLY | GF_ISOM_NALU_EXTRACT_TILE_ONLY;
				break;
			default:
				break;
			}
		}

		if (ch->nalu_extract_mode) {
			gf_isom_set_nalu_extract_mode(ch->owner->mov, ch->track, ch->nalu_extract_mode);
		}
		return ch;
	}
	if (ch->owner->nocrypt) {
		ch->is_encrypted = GF_FALSE;
		return ch;
	}
	ch->is_encrypted = GF_TRUE;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_STREAM_TYPE);
	if (p) gf_filter_pid_set_property(pid, GF_PROP_PID_ORIG_STREAM_TYPE, &PROP_UINT(p->value.uint) );

	gf_filter_pid_set_property(pid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_ENCRYPTED) );

	isor_set_crypt_config(ch);

	if (ch->nalu_extract_mode) {
		if (ch->is_encrypted) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[IsoMedia] using sample NAL rewrite with encryption is not yet supported, patch welcome\n"));
		} else {
			gf_isom_set_nalu_extract_mode(ch->owner->mov, ch->track, ch->nalu_extract_mode);
		}
	}
	return ch;
}