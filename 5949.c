GF_Err vmhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;

	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->reserved = gf_bs_read_u64(bs);
	return GF_OK;
}