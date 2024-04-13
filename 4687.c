i64 fmtutil_hlp_get_cus_p(dbuf *f, i64 *ppos)
{
	i64 x1, x2;

	x1 = (i64)dbuf_getbyte_p(f, ppos);
	if(x1%2 == 0) {
		// If it's even, divide by two.
		return x1>>1;
	}
	// If it's odd, divide by two, and add 128 times the value of
	// the next byte.
	x2 = (i64)dbuf_getbyte_p(f, ppos);
	return (x1>>1) | (x2<<7);
}