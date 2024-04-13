GF_Err diST_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_DIMSScriptTypesBox *p = (GF_DIMSScriptTypesBox *)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	if (p->content_script_types)
		gf_bs_write_data(bs, p->content_script_types, (u32) strlen(p->content_script_types)+1);
	else
		gf_bs_write_u8(bs, 0);
	return GF_OK;
}