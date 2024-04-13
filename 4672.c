static void do_iff_anno(deark *c, struct de_iffctx *ictx, i64 pos, i64 len)
{
	i64 foundpos;

	if(len<1) return;

	// Some ANNO chunks seem to be padded with one or more NUL bytes. Probably
	// best not to save them.
	if(dbuf_search_byte(ictx->f, 0x00, pos, len, &foundpos)) {
		len = foundpos - pos;
	}
	if(len<1) return;
	if(c->extract_level>=2) {
		dbuf_create_file_from_slice(ictx->f, pos, len, "anno.txt", NULL, DE_CREATEFLAG_IS_AUX);
	}
	else {
		de_ucstring *s = NULL;
		s = ucstring_create(c);
		dbuf_read_to_ucstring_n(ictx->f, pos, len, DE_DBG_MAX_STRLEN, s, 0, ictx->input_encoding);
		de_dbg(c, "annotation: \"%s\"", ucstring_getpsz(s));
		ucstring_destroy(s);
	}
}