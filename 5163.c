GF_Err styl_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_TextStyleBox*ptr = (GF_TextStyleBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, ptr->entry_count);
	for (i=0; i<ptr->entry_count; i++) gpp_write_style(bs, &ptr->styles[i]);
	return GF_OK;
}