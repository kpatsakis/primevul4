static size_t consume_str (ut8 *buf, ut8 *max, size_t sz, char *out, ut32 *offset) {
	if (!buf || !max || !out || !sz) {
		return 0;
	}
	if (!(buf + sz < max)) {
		return 0;
	}
	strncpy ((char*)out, (char*)buf, R_MIN (R_BIN_WASM_STRING_LENGTH-1, sz));
 	if (offset) *offset += sz;
 	return sz;
 }
