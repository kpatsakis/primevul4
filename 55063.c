R_API int r_flag_count(RFlag *f, const char *glob) {
	int count = 0;
	RFlagItem *flag;
	RListIter *iter;
	r_list_foreach (f->flags, iter, flag) {
		if (r_str_glob (flag->name, glob))
			count ++;
	}
	return count;
}
