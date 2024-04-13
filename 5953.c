static GF_Err ctrn_box_size(GF_TrackFragmentRunBox *ctrn)
{
	Bool use_ctso_multi = GF_TRUE;
	u32 i, count;
	GF_TrunEntry *ent;

	ctrn->ctrn_flags = 0;
	ctrn->ctrn_first_dur = ctrn->ctrn_first_size = ctrn->ctrn_first_sample_flags = ctrn->ctrn_first_ctts = 0;
	ctrn->ctrn_dur = ctrn->ctrn_size = ctrn->ctrn_sample_flags = ctrn->ctrn_ctts = 0;

	ctrn->size += 2; //16 bits for sample count
	if (ctrn->flags & GF_ISOM_TRUN_DATA_OFFSET) {
		ctrn->ctrn_flags |= GF_ISOM_TRUN_DATA_OFFSET;
		if (ABS(ctrn->data_offset) < 32767) {
			ctrn->size += 2;
			ctrn->ctrn_flags |= GF_ISOM_CTRN_DATAOFFSET_16;
		} else
			ctrn->size += 4;
	}

	count = gf_list_count(ctrn->entries);
	if (ctrn->ctso_multiplier && (ctrn->flags & GF_ISOM_TRUN_CTS_OFFSET) && (ctrn->ctso_multiplier<=0xFFFF) ) {
		for (i=0; i<count; i++) {
			GF_TrunEntry *a_ent = gf_list_get(ctrn->entries, i);
			if (a_ent->CTS_Offset % ctrn->ctso_multiplier) {
				use_ctso_multi = GF_FALSE;
				break;
			}
		}
	} else {
		use_ctso_multi = GF_FALSE;
	}
	if (ctrn->use_inherit) {
		use_ctso_multi = GF_FALSE;
		ctrn->ctrn_flags |= 0xB0; //duration=1,size=0,flags=1,cts=1 << 4
	}

	if (use_ctso_multi) {
		ctrn->size += 2;
		ctrn->ctrn_flags |= GF_ISOM_CTRN_CTSO_MULTIPLIER;
	} else {
		ctrn->ctso_multiplier = 0;
	}

	/*we always write first sample using first flags*/
	ent = gf_list_get(ctrn->entries, 0);
	ctrn->ctrn_flags |= GF_ISOM_CTRN_FIRST_SAMPLE;

	if (!ctrn->use_inherit && (ctrn->flags & GF_ISOM_TRUN_DURATION)) {
		ctrn->ctrn_first_dur = ctrn_u32_to_index(ent->Duration);
		if (ctrn->ctrn_first_dur) {
			ctrn->size += ctrn_field_size(ctrn->ctrn_first_dur);
			ctrn->ctrn_flags |= ctrn->ctrn_first_dur<<22;
		}
	}

	if (ctrn->flags & GF_ISOM_TRUN_SIZE) {
		ctrn->ctrn_first_size = ctrn_u32_to_index(ent->size);
		if (ctrn->ctrn_first_size) {
			ctrn->size += ctrn_field_size(ctrn->ctrn_first_size);
			ctrn->ctrn_flags |= ctrn->ctrn_first_size<<20;
		}
	}

	if (!ctrn->use_inherit && (ctrn->flags & GF_ISOM_TRUN_FLAGS)) {
		ctrn->ctrn_first_sample_flags = ctrn_sample_flags_to_index(ent->flags);
		if (ctrn->ctrn_first_sample_flags) {
			ctrn->size += ctrn_field_size(ctrn->ctrn_first_sample_flags);
			ctrn->ctrn_flags |= ctrn->ctrn_first_sample_flags<<18;
		}
	}
	if (!ctrn->use_inherit && (ctrn->flags & GF_ISOM_TRUN_CTS_OFFSET)) {
		ctrn->ctrn_first_ctts = ctrn_ctts_to_index(ctrn, ent->CTS_Offset);
		if (ctrn->ctrn_first_ctts) {
			ctrn->size += ctrn_field_size(ctrn->ctrn_first_ctts);
			ctrn->ctrn_flags |= ctrn->ctrn_first_ctts<<16;
		}
	}

	for (i=1; i<count; i++) {
		u8 field_idx;
		GF_TrunEntry *a_ent = gf_list_get(ctrn->entries, i);

		if (!ctrn->use_inherit && (ctrn->flags & GF_ISOM_TRUN_DURATION)) {
			field_idx = ctrn_u32_to_index(a_ent->Duration);
			if (ctrn->ctrn_dur < field_idx)
				ctrn->ctrn_dur = field_idx;
		}
		if (ctrn->flags & GF_ISOM_TRUN_SIZE) {
			field_idx = ctrn_u32_to_index(a_ent->size);
			if (ctrn->ctrn_size < field_idx)
				ctrn->ctrn_size = field_idx;
		}
		if (!ctrn->use_inherit && (ctrn->flags & GF_ISOM_TRUN_FLAGS)) {
			field_idx = ctrn_sample_flags_to_index(a_ent->flags);
			if (ctrn->ctrn_sample_flags < field_idx)
				ctrn->ctrn_sample_flags = field_idx;
		}
		if (!ctrn->use_inherit) {
			field_idx = ctrn_ctts_to_index(ctrn, a_ent->CTS_Offset);
			if (ctrn->ctrn_ctts < field_idx)
				ctrn->ctrn_ctts = field_idx;
		}
	}
	count-=1;
	if (ctrn->ctrn_dur) {
		ctrn->size += count * ctrn_field_size(ctrn->ctrn_dur);
		ctrn->ctrn_flags |= ctrn->ctrn_dur<<14;
	}
	if (ctrn->ctrn_size) {
		ctrn->size += count * ctrn_field_size(ctrn->ctrn_size);
		ctrn->ctrn_flags |= ctrn->ctrn_size<<12;
	}
	if (ctrn->ctrn_sample_flags) {
		ctrn->size += count * ctrn_field_size(ctrn->ctrn_sample_flags);
		ctrn->ctrn_flags |= ctrn->ctrn_sample_flags<<10;
	}
	if (ctrn->ctrn_ctts) {
		ctrn->size += count * ctrn_field_size(ctrn->ctrn_ctts);
		ctrn->ctrn_flags |= ctrn->ctrn_ctts<<8;
	}
	return GF_OK;
}