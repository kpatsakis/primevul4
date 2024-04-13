void gpp_write_style(GF_BitStream *bs, GF_StyleRecord *rec)
{
	gf_bs_write_u16(bs, rec->startCharOffset);
	gf_bs_write_u16(bs, rec->endCharOffset);
	gf_bs_write_u16(bs, rec->fontID);
	gf_bs_write_u8(bs, rec->style_flags);
	gf_bs_write_u8(bs, rec->font_size);
	gpp_write_rgba(bs, rec->text_color);
}