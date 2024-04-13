void fmtutil_handle_plist(deark *c, dbuf *f, i64 pos, i64 len,
	de_finfo *fi, unsigned int flags)
{
	if(de_get_ext_option_bool(c, "extractplist", 0)) {
		dbuf_create_file_from_slice(f, pos, len,
			fi?NULL:"plist", fi, DE_CREATEFLAG_IS_AUX);
		return;
	}

	de_run_module_by_id_on_slice(c, "plist", NULL, f, pos, len);
}