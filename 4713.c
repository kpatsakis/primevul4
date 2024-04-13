double dbuf_fmtutil_read_fixed_16_16(dbuf *f, i64 pos)
{
	i64 n;
	n = dbuf_geti32be(f, pos);
	return ((double)n)/65536.0;
}