GF_Err st3d_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Stereo3DBox *ptr = (GF_Stereo3DBox *)s;
	ISOM_DECREASE_SIZE(ptr, 1)
	ptr->stereo_type = gf_bs_read_u8(bs);
	return GF_OK;
}