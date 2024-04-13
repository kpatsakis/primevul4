i64 fmtutil_hlp_get_csl_p(dbuf *f, i64 *ppos)
{
	i64 x1, x2;

	x1 = dbuf_getu16le_p(f, ppos);

	if(x1%2 == 0) {
		// If it's even, divide by two, and subtract 16384
		return (x1>>1) - 16384;
	}
	// If it's odd, divide by two, add 32768 times the value of
	// the next two bytes, and subtract 67108864.
	x1 >>= 1;
	x2 = dbuf_getu16le_p(f, ppos);
	x1 += x2*32768;
	x1 -= 67108864;
	return x1;
}