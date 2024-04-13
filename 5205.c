void gpp_read_box(GF_BitStream *bs, GF_BoxRecord *rec)
{
	rec->top = gf_bs_read_u16(bs);
	rec->left = gf_bs_read_u16(bs);
	rec->bottom = gf_bs_read_u16(bs);
	rec->right = gf_bs_read_u16(bs);
}