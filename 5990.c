GF_Err saio_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SampleAuxiliaryInfoOffsetBox *ptr = (GF_SampleAuxiliaryInfoOffsetBox *)s;

	if (ptr->flags & 1) {
		ISOM_DECREASE_SIZE(ptr, 8);
		ptr->aux_info_type = gf_bs_read_u32(bs);
		ptr->aux_info_type_parameter = gf_bs_read_u32(bs);
	}
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->entry_count = gf_bs_read_u32(bs);

	if (ptr->entry_count) {
		u32 i;
		if (ptr->size / (ptr->version == 0 ? 4 : 8) < ptr->entry_count || (u64)ptr->entry_count > (u64)SIZE_MAX/sizeof(u64))
			return GF_ISOM_INVALID_FILE;
		ptr->offsets = gf_malloc(sizeof(u64)*ptr->entry_count);
		if (!ptr->offsets)
			return GF_OUT_OF_MEM;
		ptr->entry_alloc = ptr->entry_count;
		if (ptr->version==0) {
			ISOM_DECREASE_SIZE(ptr, 4*ptr->entry_count);
			for (i=0; i<ptr->entry_count; i++)
				ptr->offsets[i] = gf_bs_read_u32(bs);
		} else {
			ISOM_DECREASE_SIZE(ptr, 8*ptr->entry_count);
			for (i=0; i<ptr->entry_count; i++)
				ptr->offsets[i] = gf_bs_read_u64(bs);
		}
	}
	return GF_OK;
}