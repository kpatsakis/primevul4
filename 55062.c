R_API int r_flag_bind(RFlag *f, RFlagBind *fb) {
	fb->f = f;
	fb->exist_at = r_flag_exist_at;
	fb->get = r_flag_get;
	fb->get_at = r_flag_get_at;
	fb->set = r_flag_set;
	fb->set_fs = r_flag_space_set;
	return 0;
}
