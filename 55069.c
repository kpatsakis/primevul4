R_API const RList* /*<RFlagItem*>*/ r_flag_get_list(RFlag *f, ut64 off) {
	const RFlagsAtOffset *item = r_flag_get_nearest_list (f, off, 0);
	return item ? item->flags : NULL;
}
