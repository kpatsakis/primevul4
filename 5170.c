GF_Err blnk_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TextBlinkBox*ptr = (GF_TextBlinkBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u16(bs, ptr->startcharoffset);
	gf_bs_write_u16(bs, ptr->endcharoffset);
	return GF_OK;
}