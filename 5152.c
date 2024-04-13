GF_Err href_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 len;
	GF_Err e;
	GF_TextHyperTextBox*ptr = (GF_TextHyperTextBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, ptr->startcharoffset);
	gf_bs_write_u16(bs, ptr->endcharoffset);
	if (ptr->URL) {
		len = (u32) strlen(ptr->URL);
		gf_bs_write_u8(bs, len);
		gf_bs_write_data(bs, ptr->URL, len);
	} else {
		gf_bs_write_u8(bs, 0);
	}
	if (ptr->URL_hint) {
		len = (u32) strlen(ptr->URL_hint);
		gf_bs_write_u8(bs, len);
		gf_bs_write_data(bs, ptr->URL_hint, len);
	} else {
		gf_bs_write_u8(bs, 0);
	}
	return GF_OK;
}