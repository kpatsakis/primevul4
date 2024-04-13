GF_Err tbox_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextBoxBox*ptr = (GF_TextBoxBox*)s;
	ISOM_DECREASE_SIZE(ptr, GPP_BOX_SIZE)
	gpp_read_box(bs, &ptr->box);
	return GF_OK;
}