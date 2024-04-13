static RList *r_bin_wasm_get_table_entries (RBinWasmObj *bin, RBinWasmSection *sec) {

	RList *ret = NULL;
	RBinWasmTableEntry *ptr = NULL;

	if (!(ret = r_list_newf ((RListFree)free))) {
		return NULL;
	}

	ut8* buf = bin->buf->buf + (ut32)sec->payload_data;
	ut32 len =  sec->payload_len;
	ut32 count = sec->count;
	ut32 i = 0, r = 0;

	while (i < len && r < count) {

		if (!(ptr = R_NEW0 (RBinWasmTableEntry))) {
			return ret;
		}

		if (!(consume_u8 (buf + i, buf + len, &ptr->element_type, &i))) {
			free (ptr);
			return ret;
		}

		if (!(consume_limits (buf + i, buf + len, &ptr->limits, &i))) {
			free (ptr);
			return ret;
		}

		r_list_append (ret, ptr);

		r += 1;

	}

	return ret;
}
