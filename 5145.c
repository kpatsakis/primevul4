GF_Err tbox_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TextBoxBox*ptr = (GF_TextBoxBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gpp_write_box(bs, &ptr->box);
	return GF_OK;
}