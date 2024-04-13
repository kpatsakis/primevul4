GF_Err ftab_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_FontTableBox *ptr = (GF_FontTableBox *)s;
	ptr->entry_count = gf_bs_read_u16(bs);
	ISOM_DECREASE_SIZE(ptr, 2);

	if (ptr->size<ptr->entry_count*3) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Corrupted ftap box, skipping\n"));
		ptr->entry_count = 0;
		return GF_OK;
	}
	ptr->fonts = (GF_FontRecord *) gf_malloc(sizeof(GF_FontRecord)*ptr->entry_count);
	if (!ptr->fonts) return GF_OUT_OF_MEM;

	memset(ptr->fonts, 0, sizeof(GF_FontRecord)*ptr->entry_count);
	for (i=0; i<ptr->entry_count; i++) {
		u32 len;
		ISOM_DECREASE_SIZE(ptr, 3);
		ptr->fonts[i].fontID = gf_bs_read_u16(bs);
		len = gf_bs_read_u8(bs);
		if (len) {
			ISOM_DECREASE_SIZE(ptr, len);
			ptr->fonts[i].fontName = (char *)gf_malloc(sizeof(char)*(len+1));
			if (!ptr->fonts[i].fontName) return GF_OUT_OF_MEM;
			gf_bs_read_data(bs, ptr->fonts[i].fontName, len);
			ptr->fonts[i].fontName[len] = 0;
		}
	}
	return GF_OK;
}