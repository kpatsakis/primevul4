i64 fmtutil_hlp_get_css_p(dbuf *f, i64 *ppos)
{
	i64 x1, x2;

	x1 = (i64)dbuf_getbyte_p(f, ppos);
	if(x1%2 == 0) {
		// If it's even, divide by two, and subtract 64
		return (x1>>1) - 64;
	}
	// If it's odd, divide by two, add 128 times the value of
	// the next byte, and subtract 16384.
	x1 >>= 1;
	x2 = (i64)dbuf_getbyte_p(f, ppos);
	x1 += x2 * 128;
	x1 -= 16384;
	return x1;
}