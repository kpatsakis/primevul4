R_API bool r_config_save_char(RConfigHold *h, ...) {
	va_list ap;
	char *key;
	if (!h->list_char) {
		h->list_char = r_list_newf ((RListFree) free);
		if (!h->list_char) {
			return false;
		}
	}
	va_start (ap, h);
	while ((key = va_arg (ap, char *))) {
		RConfigHoldChar *hc = R_NEW0 (RConfigHoldChar);
		if (!hc) {
			continue;
		}
		hc->key = key;
		hc->value = r_config_get (h->cfg, key);
		r_list_append (h->list_char, hc);
	}
	va_end (ap);
	return true;
}
