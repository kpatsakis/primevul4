void fmtutil_macbitmap_read_baseaddr(deark *c, dbuf *f, struct fmtutil_macbitmap_info *bi, i64 pos)
{
	i64 n;
	de_dbg(c, "baseAddr part of PixMap, at %d", (int)pos);
	de_dbg_indent(c, 1);
	n = dbuf_getu32be(f, pos);
	de_dbg(c, "baseAddr: 0x%08x", (unsigned int)n);
	de_dbg_indent(c, -1);
}