R_API RFlagItem *r_flag_get(RFlag *f, const char *name) {
	RFlagItem *r;
	if (!f) {
		return NULL;
	}
	r = ht_find (f->ht_name, name, NULL);
	return evalFlag (f, r);
}
