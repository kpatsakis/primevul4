GF_Err twrp_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextWrapBox*ptr = (GF_TextWrapBox*)s;
	ISOM_DECREASE_SIZE(ptr, 1)
	ptr->wrap_flag = gf_bs_read_u8(bs);
	return GF_OK;
}