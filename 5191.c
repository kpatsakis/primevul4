GF_Err tx3g_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_Tx3gSampleEntryBox *ptr = (GF_Tx3gSampleEntryBox*)s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, ptr->reserved, 6);
	gf_bs_write_u16(bs, ptr->dataReferenceIndex);
	gf_bs_write_u32(bs, ptr->displayFlags);
	gf_bs_write_u8(bs, ptr->horizontal_justification);
	gf_bs_write_u8(bs, ptr->vertical_justification);
	gpp_write_rgba(bs, ptr->back_color);
	gpp_write_box(bs, &ptr->default_box);
	gpp_write_style(bs, &ptr->default_style);
	return GF_OK;
}