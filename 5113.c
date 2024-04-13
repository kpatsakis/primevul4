GF_Err ftab_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_FontTableBox *ptr = (GF_FontTableBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u16(bs, ptr->entry_count);
	for (i=0; i<ptr->entry_count; i++) {
		gf_bs_write_u16(bs, ptr->fonts[i].fontID);
		if (ptr->fonts[i].fontName) {
			u32 len = (u32) strlen(ptr->fonts[i].fontName);
			gf_bs_write_u8(bs, len);
			gf_bs_write_data(bs, ptr->fonts[i].fontName, len);
		} else {
			gf_bs_write_u8(bs, 0);
		}
	}
	return GF_OK;
}