GF_Err hlit_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextHighlightBox *ptr = (GF_TextHighlightBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->startcharoffset = gf_bs_read_u16(bs);
	ptr->endcharoffset = gf_bs_read_u16(bs);
	return GF_OK;
}