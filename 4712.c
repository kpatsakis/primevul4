void fmtutil_handle_exif2(deark *c, i64 pos, i64 len,
	u32 *returned_flags, u32 *orientation, u32 *exifversion)
{
	int user_opt;
	de_module_params *mparams = NULL;

	if(returned_flags) {
		*returned_flags = 0;
	}

	user_opt = de_get_ext_option_bool(c, "extractexif", -1);
	if(user_opt==1 || (c->extract_level>=2 && user_opt!=0)) {
		// Writing raw Exif data isn't very useful, but do so if requested.
		dbuf_create_file_from_slice(c->infile, pos, len, "exif.tif", NULL, DE_CREATEFLAG_IS_AUX);

		// Caller will have to reprocess the Exif file to extract anything from it.
		return;
	}

	mparams = de_malloc(c, sizeof(de_module_params));
	mparams->in_params.codes = "E";

	de_run_module_by_id_on_slice(c, "tiff", mparams, c->infile, pos, len);
	if(returned_flags) {
		// FIXME: It's an unfortunate bug that returned_flags does not work if
		// extract_level>=2, but for now there's no reasonable way to fix it.
		// We have to process -- not extract -- the Exif chunk if we want to
		// know what's in it.
		*returned_flags = mparams->out_params.flags;
		if((mparams->out_params.flags & 0x20) && orientation) {
			*orientation = mparams->out_params.uint1;
		}

		if((mparams->out_params.flags & 0x40) && exifversion) {
			*exifversion = mparams->out_params.uint2;
		}
	}

	de_free(c, mparams);
}