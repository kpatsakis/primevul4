R_API int r_flag_unset_name(RFlag *f, const char *name) {
	RFlagItem *item = ht_find (f->ht_name, name, NULL);
	return item && r_flag_unset (f, item);
}
