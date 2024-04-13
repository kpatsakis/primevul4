void isor_sai_bytes_removed(ISOMChannel *ch, u32 pos, u32 removed)
{
	u32 offset = 0;
	u8 *sai;
	u32 sai_size, cur_pos;
	u32 sub_count_size = 0;
	u32 i, subs_count = 0;

	if (!ch->cenc_ki || !ch->sai_buffer) return;

	sai = ch->sai_buffer;
	sai_size = ch->sai_buffer_size;

	//multikey
	if (ch->cenc_ki->value.data.ptr[0]) {
		u32 remain;
		u32 j, nb_iv_init = sai[0];
		nb_iv_init <<= 8;
		nb_iv_init |= sai[1];
		u8 *sai_p = sai + 2;
		remain = sai_size-2;

		for (j=0; j<nb_iv_init; j++) {
			u32 mk_iv_size;
			u32 idx = sai_p[0];
			idx<<=8;
			idx |= sai_p[1];

			mk_iv_size = key_info_get_iv_size(ch->cenc_ki->value.data.ptr, ch->cenc_ki->value.data.size, idx, NULL, NULL);
			mk_iv_size += 2; //idx
			if (mk_iv_size > remain) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MP4Mux] Invalid multi-key CENC SAI, cannot modify first subsample !\n"));
				return;
			}
			sai_p += mk_iv_size;
			remain -= mk_iv_size;
		}
		offset = (u32) (sai_p - sai);
		sub_count_size = 4; //32bit sub count

	} else {
		offset = key_info_get_iv_size(ch->cenc_ki->value.data.ptr, ch->cenc_ki->value.data.size, 1, NULL, NULL);
		sub_count_size = 2; //16bit sub count
	}

	sai += offset;
	if (sub_count_size==2) {
		subs_count = ((u32) sai[0]) << 8 | sai[1];
	} else {
		subs_count = GF_4CC(sai[0], sai[1], sai[2], sai[3]);
	}
	sai += sub_count_size;
	sai_size -= offset + sub_count_size;
	cur_pos = 0;
	for (i=0; i<subs_count; i++) {
		if (sai_size<6)
			return;
		u32 clear = ((u32) sai[0]) << 8 | sai[1];
		u32 crypt = GF_4CC(sai[2], sai[3], sai[4], sai[5]);
		if (cur_pos + clear > pos) {
			clear -= removed;
			sai[0] = (clear>>8) & 0xFF;
			sai[1] = (clear) & 0xFF;
			return;
		}
		cur_pos += clear + crypt;
		sai += 6;
	}
}