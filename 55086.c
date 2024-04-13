static void remove_offsetmap(RFlag *f, RFlagItem *item) {
	RFlagsAtOffset *flags = r_flag_get_nearest_list (f, item->offset, 0);
	if (flags) {
		r_list_delete_data (flags->flags, item);
		if (r_list_empty (flags->flags)) {
			r_skiplist_delete (f->by_off, flags);
		}
	}
}
