void fmtutil_handle_SAUCE(deark *c, dbuf *f, struct de_SAUCE_info *si)
{
	de_module_params mparams;

	de_zeromem(&mparams, sizeof(de_module_params));
	mparams.out_params.obj1 = (void*)si;
	de_run_module_by_id_on_slice(c, "sauce", &mparams, f, 0, f->len);
}