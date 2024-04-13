R_API int r_flag_move(RFlag *f, ut64 at, ut64 to) {
	RFlagItem *item = r_flag_get_i (f, at);
	if (item) {
		r_flag_set (f, item->name, to, item->size);
		return true;
	}
	return false;
}
