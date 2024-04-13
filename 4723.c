int fmtutil_detect_SAUCE(deark *c, dbuf *f, struct de_SAUCE_detection_data *sdd,
	unsigned int flags)
{
	de_zeromem(sdd, sizeof(struct de_SAUCE_detection_data));
	if(f->len<128) return 0;
	if(dbuf_memcmp(f, f->len-128, "SAUCE00", 7)) return 0;
	if(flags & 0x1) {
		de_dbg(c, "SAUCE metadata, signature at %"I64_FMT, f->len-128);
	}
	sdd->has_SAUCE = 1;
	sdd->data_type = dbuf_getbyte(f, f->len-128+94);
	sdd->file_type = dbuf_getbyte(f, f->len-128+95);
	return (int)sdd->has_SAUCE;
}