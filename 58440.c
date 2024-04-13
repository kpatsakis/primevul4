static size_t consume_limits (ut8 *buf, ut8 *max, struct r_bin_wasm_resizable_limits_t *out, ut32 *offset) {
	ut32 i = 0;
	if (!(consume_u8 (buf + i, max, &out->flags, &i))) return 0;
	if (!(consume_u32 (buf + i, max, &out->initial, &i))) return 0;
	if (out->flags && (!(consume_u32 (buf + i, max, &out->maximum, &i)))) return 0;
	if (offset) *offset += i;
	return i;
}
