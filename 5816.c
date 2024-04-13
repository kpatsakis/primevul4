static void isor_replace_nal(ISOMChannel *ch, u8 *data, u32 size, u8 nal_type, u32 *reset_state)
{
	u32 i, count, state=0;
	GF_NALUFFParam *sl;
	GF_List *list=NULL;
	if (ch->avcc) {
		if (nal_type==GF_AVC_NALU_PIC_PARAM) {
			list = ch->avcc->pictureParameterSets;
			state=RESET_STATE_PPS;
		} else if (nal_type==GF_AVC_NALU_SEQ_PARAM) {
			list = ch->avcc->sequenceParameterSets;
			state=RESET_STATE_SPS;
		} else if (nal_type==GF_AVC_NALU_SEQ_PARAM_EXT) {
			list = ch->avcc->sequenceParameterSetExtensions;
			state=RESET_STATE_SPS_EXT;
		} else return;
	}
	else if (ch->hvcc) {
		GF_NALUFFParamArray *hvca=NULL;
		count = gf_list_count(ch->hvcc->param_array);
		for (i=0; i<count; i++) {
			hvca = gf_list_get(ch->hvcc->param_array, i);
			if (hvca->type==nal_type) {
				list = hvca->nalus;
				break;
			}
			hvca = NULL;
		}
		if (!hvca) {
			GF_SAFEALLOC(hvca, GF_NALUFFParamArray);
			if (hvca) {
				list = hvca->nalus = gf_list_new();
				hvca->type = nal_type;
				gf_list_add(ch->hvcc->param_array, hvca);
			}
		}
		switch (nal_type) {
		case GF_HEVC_NALU_VID_PARAM:
			state = RESET_STATE_VPS;
			break;
		case GF_HEVC_NALU_SEQ_PARAM:
			state = RESET_STATE_SPS;
			break;
		case GF_HEVC_NALU_PIC_PARAM:
			state = RESET_STATE_PPS;
			break;
		}
	}
	else if (ch->vvcc) {
		GF_NALUFFParamArray *vvca=NULL;
		count = gf_list_count(ch->vvcc->param_array);
		for (i=0; i<count; i++) {
			vvca = gf_list_get(ch->vvcc->param_array, i);
			if (vvca->type==nal_type) {
				list = vvca->nalus;
				break;
			}
			vvca = NULL;
		}
		if (!vvca) {
			GF_SAFEALLOC(vvca, GF_NALUFFParamArray);
			if (vvca) {
				list = vvca->nalus = gf_list_new();
				vvca->type = nal_type;
				gf_list_add(ch->vvcc->param_array, vvca);
			}
		}
		switch (nal_type) {
		case GF_VVC_NALU_VID_PARAM:
			state = RESET_STATE_VPS;
			break;
		case GF_VVC_NALU_SEQ_PARAM:
			state = RESET_STATE_SPS;
			break;
		case GF_VVC_NALU_PIC_PARAM:
			state = RESET_STATE_PPS;
			break;
		case GF_VVC_NALU_DEC_PARAM:
			state = RESET_STATE_DCI;
			break;
		}
	}

	ch->xps_mask |= state;

	count = gf_list_count(list);
	for (i=0; i<count; i++) {
		sl = gf_list_get(list, i);
		if ((sl->size==size) && !memcmp(sl->data, data, size)) return;
	}
	if (! (*reset_state & state))  {
		isor_reset_seq_list(list);
		*reset_state |= state;
	}
	GF_SAFEALLOC(sl, GF_NALUFFParam);
	if (!sl) return;
	sl->data = gf_malloc(sizeof(char)*size);
	memcpy(sl->data, data, size);
	sl->size = size;
	gf_list_add(list, sl);
}