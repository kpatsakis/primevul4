R_API RFlagItem *r_flag_get_i(RFlag *f, ut64 off) {
	const RList *list;
	if (!f) {
		return NULL;
	}
	list = r_flag_get_list (f, off);
	return list ? evalFlag (f, r_list_get_top (list)) : NULL;
}
