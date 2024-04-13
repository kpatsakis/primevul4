void gpp_read_style(GF_BitStream *bs, GF_StyleRecord *rec)
{
	rec->startCharOffset = gf_bs_read_u16(bs);
	rec->endCharOffset = gf_bs_read_u16(bs);
	rec->fontID = gf_bs_read_u16(bs);
	rec->style_flags = gf_bs_read_u8(bs);
	rec->font_size = gf_bs_read_u8(bs);
	rec->text_color = gpp_read_rgba(bs);
}