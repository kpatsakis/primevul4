void fmtutil_handle_photoshop_rsrc(deark *c, dbuf *f, i64 pos, i64 len,
	unsigned int flags)
{
	fmtutil_handle_photoshop_rsrc2(c, f, pos, len, flags, NULL);
}