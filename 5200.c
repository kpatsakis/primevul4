GF_Err href_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 len;
	GF_TextHyperTextBox*ptr = (GF_TextHyperTextBox*)s;
	ISOM_DECREASE_SIZE(ptr, 6) //including 2 length fields
	ptr->startcharoffset = gf_bs_read_u16(bs);
	ptr->endcharoffset = gf_bs_read_u16(bs);
	len = gf_bs_read_u8(bs);
	if (len) {
		ISOM_DECREASE_SIZE(ptr, len)
		ptr->URL = (char *) gf_malloc(sizeof(char) * (len+1));
		if (!ptr->URL) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->URL, len);
		ptr->URL[len] = 0;
	}
	len = gf_bs_read_u8(bs);
	if (len) {
		ISOM_DECREASE_SIZE(ptr, len)
		ptr->URL_hint = (char *) gf_malloc(sizeof(char) * (len+1));
		if (!ptr->URL_hint) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->URL_hint, len);
		ptr->URL_hint[len]= 0;
	}
	return GF_OK;
}