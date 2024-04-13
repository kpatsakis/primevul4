GF_Err pasp_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_PixelAspectRatioBox *ptr = (GF_PixelAspectRatioBox *)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->hSpacing);
	gf_bs_write_u32(bs, ptr->vSpacing);
	return GF_OK;
}