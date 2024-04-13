GF_Err hclr_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TextHighlightColorBox*ptr = (GF_TextHighlightColorBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gpp_write_rgba(bs, ptr->hil_color);
	return GF_OK;
}