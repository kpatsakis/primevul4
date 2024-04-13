void gpp_write_box(GF_BitStream *bs, GF_BoxRecord *rec)
{
	gf_bs_write_u16(bs, rec->top);
	gf_bs_write_u16(bs, rec->left);
	gf_bs_write_u16(bs, rec->bottom);
	gf_bs_write_u16(bs, rec->right);
}