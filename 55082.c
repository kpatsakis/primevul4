R_API void r_flag_unset_all(RFlag *f) {
	f->space_idx = -1;
	r_list_free (f->flags);
	f->flags = r_list_newf ((RListFree)r_flag_item_free);
	if (!f->flags) {
		return;
	}
	ht_free (f->ht_name);
	f->ht_name = ht_new (NULL, flag_free_kv, NULL);
	r_skiplist_purge (f->by_off);
	r_flag_space_unset (f, NULL);
}
