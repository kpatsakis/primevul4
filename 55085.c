R_API int r_flag_unset_off(RFlag *f, ut64 off) {
	RFlagItem *item = r_flag_get_i (f, off);
	if (item && r_flag_unset (f, item)) {
		return true;
	}
	return false;
}
