u32 gpp_read_rgba(GF_BitStream *bs)
{
	u8 r, g, b, a;
	u32 col;
	r = gf_bs_read_u8(bs);
	g = gf_bs_read_u8(bs);
	b = gf_bs_read_u8(bs);
	a = gf_bs_read_u8(bs);
	col = a;
	col<<=8;
	col |= r;
	col<<=8;
	col |= g;
	col<<=8;
	col |= b;
	return col;
}