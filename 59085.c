static char *getstr(RBinDexObj *bin, int idx) {
	ut8 buf[6];
	ut64 len;
	int uleblen;
	if (!bin || idx < 0 || idx >= bin->header.strings_size ||
		!bin->strings) {
		return NULL;
	}
	if (bin->strings[idx] >= bin->size) {
		return NULL;
	}

	if (r_buf_read_at (bin->b, bin->strings[idx], buf, sizeof (buf)) < 1) {
		return NULL;
	}
	uleblen = r_uleb128 (buf, sizeof (buf), &len) - buf;
	if (!uleblen || uleblen >= bin->size) {
		return NULL;
	}
	if (!len || len >= bin->size) {
		return NULL;
	}
	char c = 'a';
	while (c) {
		ut64 offset = bin->strings[idx] + uleblen + len;
		if (offset >= bin->size || offset < len) {
			return NULL;
		}
		r_buf_read_at (bin->b, offset, (ut8*)&c, 1);
		len++;
	}
	if ((int)len > 0 && len < R_BIN_SIZEOF_STRINGS) {
		char *str = calloc (1, len + 1);
		if (str) {
			r_buf_read_at (bin->b, (bin->strings[idx]) + uleblen,
						(ut8 *)str, len);
			str[len] = 0;
			return str;
		}
	}
	return NULL;
}
