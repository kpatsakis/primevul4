static void do_iff_text_chunk(deark *c, struct de_iffctx *ictx, i64 dpos, i64 dlen,
	const char *name)
{
	de_ucstring *s = NULL;

	if(dlen<1) return;
	s = ucstring_create(c);
	dbuf_read_to_ucstring_n(ictx->f,
		dpos, dlen, DE_DBG_MAX_STRLEN,
		s, DE_CONVFLAG_STOP_AT_NUL, ictx->input_encoding);
	de_dbg(c, "%s: \"%s\"", name, ucstring_getpsz(s));
	ucstring_destroy(s);
}