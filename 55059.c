static void flag_skiplist_free(void *data) {
	RFlagsAtOffset *item = (RFlagsAtOffset *)data;
	r_list_free (item->flags);
	free (data);
}
