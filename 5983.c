GF_Err stvi_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_StereoVideoBox *ptr = (GF_StereoVideoBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_int(bs, 0, 30);
	gf_bs_write_int(bs, ptr->single_view_allowed, 2);
	gf_bs_write_u32(bs, ptr->stereo_scheme);
	gf_bs_write_u32(bs, ptr->sit_len);
	gf_bs_write_data(bs, ptr->stereo_indication_type, ptr->sit_len);

	return GF_OK;
}