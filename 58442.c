static size_t consume_s32 (ut8 *buf, ut8 *max, st32 *out, ut32 *offset) {
	size_t n;
	if (!buf || !max || !out) {
		return 0;
	}
	if (!(n = read_i32_leb128 (buf, max, out)) || n > 5) {
		return 0;
	}
	if (offset) {
		*offset += n;
	}
	return n;
}
