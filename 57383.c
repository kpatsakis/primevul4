R_API bool r_config_save_num(RConfigHold *h, ...) {
	va_list ap;
	char *key;
	if (!h->list_num) {
		h->list_num = r_list_newf ((RListFree) free);
		if (!h->list_num) {
			return false;
		}
	}
	va_start (ap, h);
	while ((key = va_arg (ap, char *))) {
		RConfigHoldNum *hc = R_NEW0 (RConfigHoldNum);
		if (!hc) {
			continue;
		}
		hc->key = key;
		hc->value = r_config_get_i (h->cfg, key);
		r_list_append (h->list_num, hc);
	}
	va_end (ap);
	return true;
}
