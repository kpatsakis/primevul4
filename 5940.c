GF_Err saiz_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SampleAuxiliaryInfoSizeBox*ptr = (GF_SampleAuxiliaryInfoSizeBox*) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	if (ptr->flags & 1) {
		gf_bs_write_u32(bs, ptr->aux_info_type);
		gf_bs_write_u32(bs, ptr->aux_info_type_parameter);
	}
	gf_bs_write_u8(bs, ptr->default_sample_info_size);
	gf_bs_write_u32(bs, ptr->sample_count);
	if (!ptr->default_sample_info_size) {
		if (!ptr->sample_info_size)
			gf_bs_write_u8(bs, 0);
		else
			gf_bs_write_data(bs, (char *) ptr->sample_info_size, ptr->sample_count);
	}
	return GF_OK;
}