GF_Err twrp_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TextWrapBox*ptr = (GF_TextWrapBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->wrap_flag);
	return GF_OK;
}