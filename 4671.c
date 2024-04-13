void fmtutil_handle_iptc(deark *c, dbuf *f, i64 pos, i64 len,
	unsigned int flags)
{
	int should_decode;
	int should_extract;
	int user_opt;
	int extract_fmt = 1; // 0=raw, 1=TIFF-wrapped

	if(len<1) return;

	user_opt = de_get_ext_option_bool(c, "extractiptc", -1);

	if(user_opt==1 || (c->extract_level>=2 && user_opt!=0)) {
		should_decode = 0;
		should_extract = 1;
		if(flags&0x2) {
			// Avoid "extracting" in a way that would just recreate the exact same file.
			extract_fmt = 0;
		}
	}
	else {
		should_decode = 1;
		should_extract = 0;
	}

	if(should_decode) {
		de_run_module_by_id_on_slice(c, "iptc", NULL, f, pos, len);
	}

	if(should_extract && extract_fmt==0) {
		dbuf_create_file_from_slice(f, pos, len, "iptc", NULL, DE_CREATEFLAG_IS_AUX);
	}
	else if(should_extract && extract_fmt==1) {
		wrap_in_tiff(c, f, pos, len, "Deark extracted IPTC", 33723, "iptctiff",
			DE_CREATEFLAG_IS_AUX);
	}
}