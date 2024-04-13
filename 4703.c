void fmtutil_macbitmap_read_rowbytes_and_bounds(deark *c, dbuf *f,
	struct fmtutil_macbitmap_info *bi, i64 pos)
{
	struct pict_rect tmprect;
	i64 rowbytes_code;

	de_dbg(c, "rowBytes/bounds part of bitmap/PixMap header, at %d", (int)pos);
	de_dbg_indent(c, 1);
	rowbytes_code = dbuf_getu16be(f, pos);
	bi->rowbytes = rowbytes_code & 0x7fff;
	bi->pixmap_flag = (rowbytes_code & 0x8000)?1:0;
	de_dbg(c, "rowBytes: %d", (int)bi->rowbytes);
	de_dbg(c, "pixmap flag: %d", bi->pixmap_flag);

	pict_read_rect(f, pos+2, &tmprect, "rect");
	bi->npwidth = tmprect.r - tmprect.l;
	bi->pdwidth = bi->npwidth; // default
	bi->height = tmprect.b - tmprect.t;

	de_dbg_indent(c, -1);
}