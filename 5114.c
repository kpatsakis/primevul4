GF_Err dlay_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TextScrollDelayBox*ptr = (GF_TextScrollDelayBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->scroll_delay);
	return GF_OK;
}