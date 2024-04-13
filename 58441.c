static size_t consume_locals (ut8 *buf, ut8 *max, ut32 count, RBinWasmCodeEntry *out, ut32 *offset) {
	ut32 i = 0, j = 0;
	if (count < 1) return 0;
	if (!(out->locals = (struct r_bin_wasm_local_entry_t*) malloc (sizeof(struct r_bin_wasm_local_entry_t) * count))) {
		return 0;
	}
	while (buf + i < max && j < count) {
		if (!(consume_u32 (buf + i, max, &out->locals[j].count, &i))) {
			free (out->locals);
			return 0;	
		}

		if (!(consume_s8 (buf + i, max, (st8*)&out->locals[j].type, &i))) {
			free (out->locals);
			return 0;
		}
		j += 1;
	}
	if (offset) *offset += i;
	return j;
}
