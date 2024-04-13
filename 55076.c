R_API RFlag * r_flag_new() {
	int i;
	RFlag *f = R_NEW0 (RFlag);
	if (!f) return NULL;
	f->num = r_num_new (&num_callback, &str_callback, f);
	if (!f->num) {
		r_flag_free (f);
		return NULL;
	}
	f->base = 0;
	f->cb_printf = (PrintfCallback)printf;
#if R_FLAG_ZONE_USE_SDB
	f->zones = sdb_new0 ();
#else
	f->zones = NULL;
#endif
	f->flags = r_list_new ();
	if (!f->flags) {
		r_flag_free (f);
		return NULL;
	}
	f->flags->free = (RListFree) r_flag_item_free;
	f->space_idx = -1;
	f->spacestack = r_list_newf (NULL);
	if (!f->spacestack) {
		r_flag_free (f);
		return NULL;
	}
	f->ht_name = ht_new (NULL, flag_free_kv, NULL);
	f->by_off = r_skiplist_new (flag_skiplist_free, flag_skiplist_cmp);
#if R_FLAG_ZONE_USE_SDB
	sdb_free (f->zones);
#else
	r_list_free (f->zones);
#endif
	for (i = 0; i < R_FLAG_SPACES_MAX; i++) {
		f->spaces[i] = NULL;
	}
	return f;
}
