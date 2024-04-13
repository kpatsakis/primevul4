GF_Err tx3g_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_Tx3gSampleEntryBox *ptr = (GF_Tx3gSampleEntryBox*)s;

	ISOM_DECREASE_SIZE(ptr, (18 + GPP_BOX_SIZE + GPP_STYLE_SIZE) );

	e = gf_isom_base_sample_entry_read((GF_SampleEntryBox *)ptr, bs);
	if (e) return e;

	ptr->displayFlags = gf_bs_read_u32(bs);
	ptr->horizontal_justification = gf_bs_read_u8(bs);
	ptr->vertical_justification = gf_bs_read_u8(bs);
	ptr->back_color = gpp_read_rgba(bs);
	gpp_read_box(bs, &ptr->default_box);
	gpp_read_style(bs, &ptr->default_style);


	return gf_isom_box_array_read(s, bs);
}