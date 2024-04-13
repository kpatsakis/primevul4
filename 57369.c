R_API void r_config_hold_free(RConfigHold *h) {
	if (h) {
		r_list_free (h->list_num);
		r_list_free (h->list_char);
		R_FREE (h);
	}
}
