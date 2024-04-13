void fmtutil_riscos_read_attribs_field(deark *c, dbuf *f, struct de_riscos_file_attrs *rfa,
	i64 pos, unsigned int flags)
{
	rfa->attribs = (u32)dbuf_getu32le(f, pos);
	de_dbg(c, "attribs: 0x%08x", (unsigned int)rfa->attribs);
	de_dbg_indent(c, 1);
	rfa->crc_from_attribs = rfa->attribs>>16;
	if(flags & DE_RISCOS_FLAG_HAS_CRC) {
		de_dbg(c, "crc (reported): 0x%04x", (unsigned int)rfa->crc_from_attribs);
	}
	if(flags & DE_RISCOS_FLAG_HAS_LZWMAXBITS) {
		rfa->lzwmaxbits = (unsigned int)((rfa->attribs&0xff00)>>8);
		de_dbg(c, "lzw maxbits: %u", rfa->lzwmaxbits);
	}
	de_dbg_indent(c, -1);
}