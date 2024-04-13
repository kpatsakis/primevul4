GF_Err csgp_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i, bits, gidx_mask;
	Bool index_msb_indicates_fragment_local_description, grouping_type_parameter_present;
	u32 pattern_size, scount_size, index_size;
	GF_CompactSampleGroupBox *ptr = (GF_CompactSampleGroupBox *)s;

	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->version = gf_bs_read_u8(bs);
	ptr->flags = gf_bs_read_u24(bs);

	index_msb_indicates_fragment_local_description = (ptr->flags & (1<<7)) ? GF_TRUE : GF_FALSE;
	grouping_type_parameter_present = (ptr->flags & (1<<6)) ? GF_TRUE : GF_FALSE;

	pattern_size = get_size_by_code( ((ptr->flags>>4) & 0x3) );
	scount_size = get_size_by_code( ((ptr->flags>>2) & 0x3) );
	index_size = get_size_by_code( (ptr->flags & 0x3) );

	if (((pattern_size==4) && (scount_size!=4)) || ((pattern_size!=4) && (scount_size==4))) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] compact sample gorup pattern_size and sample_count_size mare not both 4 bits\n"));
		return GF_ISOM_INVALID_FILE;
	}

	ptr->grouping_type = gf_bs_read_u32(bs);
	if (grouping_type_parameter_present) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->grouping_type_parameter = gf_bs_read_u32(bs);
	}
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->pattern_count = gf_bs_read_u32(bs);


	if ( (ptr->size / ( (pattern_size + scount_size) / 8 ) < ptr->pattern_count) || (u64)ptr->pattern_count > (u64)SIZE_MAX/sizeof(GF_CompactSampleGroupPattern) ) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] compact sample gorup pattern_count value (%lu) invalid\n", ptr->pattern_count));
		return GF_ISOM_INVALID_FILE;
	}

	ptr->patterns = gf_malloc(sizeof(GF_CompactSampleGroupPattern) * ptr->pattern_count);
	if (!ptr->patterns) return GF_OUT_OF_MEM;
	memset(ptr->patterns, 0, sizeof(GF_CompactSampleGroupPattern) * ptr->pattern_count);

	u64 patterns_sizes=0;
	bits = 0;
	for (i=0; i<ptr->pattern_count; i++) {
		ptr->patterns[i].length = gf_bs_read_int(bs, pattern_size);
		ptr->patterns[i].sample_count = gf_bs_read_int(bs, scount_size);
		bits += pattern_size + scount_size;
		if (! (bits % 8)) {
			bits/=8;
			ISOM_DECREASE_SIZE(ptr, bits);
			bits=0;
		}
		patterns_sizes+=ptr->patterns[i].length;
		if (patterns_sizes * index_size > ptr->size*8) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] compact sample gorup pattern cumulated sizes "LLU" larger than box size "LLU"\n", patterns_sizes, ptr->size));
			ptr->patterns[i].sample_group_description_indices = NULL;
			return GF_ISOM_INVALID_FILE;
		}

		if ( (u64)ptr->patterns[i].length > (u64)SIZE_MAX/sizeof(u32) ) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] compact sample gorup pattern #%d value (%lu) invalid\n", i, ptr->patterns[i].length));
			ptr->patterns[i].sample_group_description_indices = NULL;
			return GF_ISOM_INVALID_FILE;
		}
		ptr->patterns[i].sample_group_description_indices = gf_malloc(sizeof(u32) * ptr->patterns[i].length);
		if (!ptr->patterns[i].sample_group_description_indices) return GF_OUT_OF_MEM;
	}
	bits = 0;
	gidx_mask = ((u32)1) << (index_size-1);
	for (i=0; i<ptr->pattern_count; i++) {
		u32 j;
		for (j=0; j<ptr->patterns[i].length; j++) {
			u32 idx = gf_bs_read_int(bs, index_size);
			if (index_msb_indicates_fragment_local_description) {
				//MSB set, this is a index of a group described in the fragment
				if (idx & gidx_mask) {
					idx += 0x10000;
					idx &= ~gidx_mask;
				}
			}
			ptr->patterns[i].sample_group_description_indices[j] = idx;
			bits += index_size;

			if (! (bits % 8)) {
				bits/=8;
				ISOM_DECREASE_SIZE(ptr, bits);
				bits=0;
			}
		}
	}
	if (bits)
		gf_bs_align(bs);
	return GF_OK;
}