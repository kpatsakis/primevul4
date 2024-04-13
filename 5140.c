GF_Err dlay_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextScrollDelayBox*ptr = (GF_TextScrollDelayBox*)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->scroll_delay = gf_bs_read_u32(bs);
	return GF_OK;
}