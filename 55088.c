static const char *str_callback(RNum *user, ut64 off, int *ok) {
	const RList *list;
	RFlag *f = (RFlag*)user;
	RFlagItem *item;
	if (ok) {
		*ok = 0;
	}
	if (f) {
		list = r_flag_get_list (f, off);
		item = r_list_get_top (list);
		if (item) {
			if (ok) {
				*ok = true;
			}
			return item->name;
		}
	}
	return NULL;
}
