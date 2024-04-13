R_API int r_flag_unset(RFlag *f, RFlagItem *item) {
	remove_offsetmap (f, item);
	ht_delete (f->ht_name, item->name);
	r_list_delete_data (f->flags, item);
	return true;
}
