void fmtutil_handle_photoshop_rsrc2(deark *c, dbuf *f, i64 pos, i64 len,
	unsigned int flags, struct de_module_out_params *oparams)
{
	int should_decode;
	int should_extract;
	int extract_fmt = 1; // 0=raw, 1=TIFF-wrapped

	if(flags&0x1) {
		should_decode = 0;
		should_extract = 1;
	}
	else if(de_get_ext_option_bool(c, "extract8bim", 0)) {
		should_extract = 1;
		should_decode = 0;
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
		de_module_params *mparams = NULL;

		mparams = de_malloc(c, sizeof(de_module_params));
		mparams->in_params.codes = "R";
		if(oparams) {
			// Since mparams->out_params is an embedded struct, not a pointer,
			// we have to copy oparam's fields to and from it.
			mparams->out_params = *oparams; // struct copy
		}
		de_run_module_by_id_on_slice(c, "psd", mparams, f, pos, len);
		if(oparams) {
			*oparams = mparams->out_params; // struct copy
		}
		de_free(c, mparams);
	}

	if(should_extract && extract_fmt==0) {
		dbuf_create_file_from_slice(f, pos, len, "8bim", NULL, DE_CREATEFLAG_IS_AUX);
	}
	else if(should_extract && extract_fmt==1) {
		wrap_in_tiff(c, f, pos, len, "Deark extracted 8BIM", 34377, "8bimtiff",
			DE_CREATEFLAG_IS_AUX);
	}
}