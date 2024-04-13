R_API void r_config_restore(RConfigHold *h) {
	RListIter *iter;
	RConfigHoldChar *hchar;
	RConfigHoldNum *hnum;
	if (h) {
		r_list_foreach (h->list_num, iter, hnum) {
			(void)r_config_set_i (h->cfg, hnum->key, hnum->value);
		}
		r_list_foreach (h->list_char, iter, hchar) {
			(void)r_config_set (h->cfg, hchar->key, hchar->value);
		}
	}
}
