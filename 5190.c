void gpp_write_rgba(GF_BitStream *bs, u32 col)
{
	gf_bs_write_u8(bs, (col>>16) & 0xFF);
	gf_bs_write_u8(bs, (col>>8) & 0xFF);
	gf_bs_write_u8(bs, (col) & 0xFF);
	gf_bs_write_u8(bs, (col>>24) & 0xFF);
}