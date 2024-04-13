GF_Err hclr_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextHighlightColorBox*ptr = (GF_TextHighlightColorBox*)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->hil_color = gpp_read_rgba(bs);
	return GF_OK;
}