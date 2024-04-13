R_API int r_flag_rename(RFlag *f, RFlagItem *item, const char *name) {
	if (!f || !item || !name || !*name) {
		return false;
	}
#if 0
	ut64 off = item->offset;
	int size = item->size;
	r_flag_unset (f, item);
	r_flag_set (f, name, off, size);
	return true;
#else
	ht_delete (f->ht_name, item->name);
	if (!set_name (item, name)) {
		return false;
	}
	ht_insert (f->ht_name, item->name, item);
#endif
	return true;
}
