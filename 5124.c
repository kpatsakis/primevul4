GF_Err styl_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_TextStyleBox*ptr = (GF_TextStyleBox*)s;
	ISOM_DECREASE_SIZE(ptr, 2);
	ptr->entry_count = gf_bs_read_u16(bs);

	if (ptr->size / GPP_STYLE_SIZE < ptr->entry_count)
		return GF_ISOM_INVALID_FILE;

	if (ptr->entry_count) {
		ptr->styles = (GF_StyleRecord*)gf_malloc(sizeof(GF_StyleRecord)*ptr->entry_count);
		if (!ptr->styles) return GF_OUT_OF_MEM;
		for (i=0; i<ptr->entry_count; i++) {
			ISOM_DECREASE_SIZE(ptr, GPP_STYLE_SIZE);
			gpp_read_style(bs, &ptr->styles[i]);
		}
	}
	return GF_OK;
}