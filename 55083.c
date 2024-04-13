R_API int r_flag_unset_glob(RFlag *f, const char *glob) {
	RListIter it, *iter;
	RFlagItem *flag;
	int n = 0;

	r_list_foreach (f->flags, iter, flag) {
		if (IS_IN_SPACE (f, flag)) continue;
		if (!glob || r_str_glob (flag->name, glob)) {
			it.n = iter->n;
			r_flag_unset (f, flag);
			iter = &it;
			n++;
		}
	}
	return n;
}
