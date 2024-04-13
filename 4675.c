void fmtutil_handle_id3(deark *c, dbuf *f, struct de_id3info *id3i,
	unsigned int flags)
{
	i64 id3v1pos = 0;
	int look_for_id3v1;

	de_zeromem(id3i, sizeof(struct de_id3info));
	id3i->main_start = 0;
	id3i->main_end = f->len;

	id3i->has_id3v2 = !dbuf_memcmp(f, 0, "ID3", 3);
	if(id3i->has_id3v2) {
		de_module_params id3v2mparams;

		de_dbg(c, "ID3v2 data at %d", 0);
		de_dbg_indent(c, 1);
		de_zeromem(&id3v2mparams, sizeof(de_module_params));
		id3v2mparams.in_params.codes = "I";
		de_run_module_by_id_on_slice(c, "id3", &id3v2mparams, f, 0, f->len);
		de_dbg_indent(c, -1);
		id3i->main_start += id3v2mparams.out_params.int64_1;
	}

	look_for_id3v1 = 1;
	if(look_for_id3v1) {
		id3v1pos = f->len-128;
		if(!dbuf_memcmp(f, id3v1pos, "TAG", 3)) {
			id3i->has_id3v1 = 1;
		}
	}

	if(id3i->has_id3v1) {
		de_module_params id3v1mparams;

		de_dbg(c, "ID3v1 data at %"I64_FMT, id3v1pos);
		de_dbg_indent(c, 1);
		de_zeromem(&id3v1mparams, sizeof(de_module_params));
		id3v1mparams.in_params.codes = "1";
		de_run_module_by_id_on_slice(c, "id3", &id3v1mparams, f, id3v1pos, 128);
		de_dbg_indent(c, -1);
		id3i->main_end = id3v1pos;
	}
}