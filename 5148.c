GF_Err blnk_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextBlinkBox*ptr = (GF_TextBlinkBox*)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->startcharoffset = gf_bs_read_u16(bs);
	ptr->endcharoffset = gf_bs_read_u16(bs);
	return GF_OK;
}