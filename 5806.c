void isor_reader_check_config(ISOMChannel *ch)
{
	u32 nalu_len, reset_state;
	if (!ch->check_hevc_ps && !ch->check_avc_ps && !ch->check_vvc_ps && !ch->check_mhas_pl) return;

	if (!ch->sample) return;
	ch->xps_mask = 0;

	//we cannot touch the payload if encrypted but not CENC !!
	if (ch->is_encrypted && !ch->is_cenc)
		return;

	if (ch->check_mhas_pl) {
		//we cannot touch the payload if encrypted !!
		if (ch->pck_encrypted) return;
		u64 ch_layout = 0;
		s32 PL = gf_mpegh_get_mhas_pl(ch->sample->data, ch->sample->dataLength, &ch_layout);
		if (PL>0) {
			gf_filter_pid_set_property(ch->pid, GF_PROP_PID_PROFILE_LEVEL, &PROP_UINT((u32) PL));
			ch->check_mhas_pl = GF_FALSE;
			if (ch_layout)
				gf_filter_pid_set_property(ch->pid, GF_PROP_PID_CHANNEL_LAYOUT, &PROP_LONGUINT(ch_layout));
		}
		return;
	}
	//analyze mode, do not rewrite
	if (ch->owner->analyze) return;

	//we cannot touch the payload if encrypted but no SAI buffer
	if (ch->pck_encrypted && !ch->sai_buffer)
		return;

	nalu_len = 4;
	if (ch->avcc) nalu_len = ch->avcc->nal_unit_size;
	else if (ch->hvcc) nalu_len = ch->hvcc->nal_unit_size;
	else if (ch->vvcc) nalu_len = ch->vvcc->nal_unit_size;

	reset_state = 0;

	if (!ch->nal_bs) ch->nal_bs = gf_bs_new(ch->sample->data, ch->sample->dataLength, GF_BITSTREAM_READ);
	else gf_bs_reassign_buffer(ch->nal_bs, ch->sample->data, ch->sample->dataLength);

	while (gf_bs_available(ch->nal_bs)) {
		Bool replace_nal = GF_FALSE;
		u8 nal_type=0;
		u32 pos = (u32) gf_bs_get_position(ch->nal_bs);
		u32 size = gf_bs_read_int(ch->nal_bs, nalu_len*8);
		//this takes care of size + pos + nalu_len > 0 but (s32) size < 0 ...
		if (ch->sample->dataLength < size) break;
		if (ch->sample->dataLength < size + pos + nalu_len) break;
		if (ch->check_avc_ps) {
			u8 hdr = gf_bs_peek_bits(ch->nal_bs, 8, 0);
			nal_type = hdr & 0x1F;
			switch (nal_type) {
			case GF_AVC_NALU_SEQ_PARAM:
			case GF_AVC_NALU_SEQ_PARAM_EXT:
			case GF_AVC_NALU_PIC_PARAM:
				replace_nal = GF_TRUE;
				break;
			}
		}
		else if (ch->check_hevc_ps) {
			u8 hdr = gf_bs_peek_bits(ch->nal_bs, 8, 0);
			nal_type = (hdr & 0x7E) >> 1;
			switch (nal_type) {
			case GF_HEVC_NALU_VID_PARAM:
			case GF_HEVC_NALU_SEQ_PARAM:
			case GF_HEVC_NALU_PIC_PARAM:
				replace_nal = GF_TRUE;
				break;
			}
		}
		else if (ch->check_vvc_ps) {
			u8 hdr = gf_bs_peek_bits(ch->nal_bs, 8, 1);
			nal_type = hdr >> 3;
			switch (nal_type) {
			case GF_VVC_NALU_VID_PARAM:
			case GF_VVC_NALU_SEQ_PARAM:
			case GF_VVC_NALU_PIC_PARAM:
			case GF_VVC_NALU_DEC_PARAM:
				replace_nal = GF_TRUE;
				break;
			}
		}
		gf_bs_skip_bytes(ch->nal_bs, size);

		if (replace_nal) {
			u32 move_size = ch->sample->dataLength - size - pos - nalu_len;
			isor_replace_nal(ch, ch->sample->data + pos + nalu_len, size, nal_type, &reset_state);
			if (move_size)
				memmove(ch->sample->data + pos, ch->sample->data + pos + size + nalu_len, ch->sample->dataLength - size - pos - nalu_len);

			ch->sample->dataLength -= size + nalu_len;
			gf_bs_reassign_buffer(ch->nal_bs, ch->sample->data, ch->sample->dataLength);
			gf_bs_seek(ch->nal_bs, pos);

			//remove nal from clear subsample range
			if (ch->pck_encrypted)
				isor_sai_bytes_removed(ch, pos, nalu_len+size);
		}
	}

	if (reset_state) {
		u8 *dsi=NULL;
		u32 dsi_size=0;
		if (ch->check_avc_ps) {
			gf_odf_avc_cfg_write(ch->avcc, &dsi, &dsi_size);
		}
		else if (ch->check_hevc_ps) {
			gf_odf_hevc_cfg_write(ch->hvcc, &dsi, &dsi_size);
		}
		else if (ch->check_vvc_ps) {
			gf_odf_vvc_cfg_write(ch->vvcc, &dsi, &dsi_size);
		}
		if (dsi && dsi_size) {
			u32 dsi_crc = gf_crc_32(dsi, dsi_size);
			if (ch->dsi_crc == dsi_crc) {
				gf_free(dsi);
			} else {
				ch->dsi_crc = dsi_crc;
				gf_filter_pid_set_property(ch->pid, GF_PROP_PID_DECODER_CONFIG, &PROP_DATA_NO_COPY(dsi, dsi_size) );
			}
		}
	}
}