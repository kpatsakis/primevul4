static ut64 num_callback(RNum *user, const char *name, int *ok) {
	RFlag *f = (RFlag*)user;
	RFlagItem *item;
	if (ok) {
		*ok = 0;
	}
	item = ht_find (f->ht_name, name, NULL);
	if (item) {
		if (item->alias) {
			return 0LL;
		}
		if (ok) {
			*ok = 1;
		}
		return item->offset;
	}
	return 0LL;
}
