static RList *r_bin_wasm_get_type_entries (RBinWasmObj *bin, RBinWasmSection *sec) {

	RList *ret = NULL;
	RBinWasmTypeEntry *ptr = NULL;

	if (!(ret = r_list_newf ((RListFree)free))) {
		return NULL;
	}

	ut8* buf = bin->buf->buf + (ut32)sec->payload_data;
	ut32 len =  sec->payload_len;
	ut32 count = sec->count;
	ut32 i = 0, r = 0;

	while (i < len && r < count) {
		if (!(ptr = R_NEW0 (RBinWasmTypeEntry))) {
			return ret;
		}

		if (!(consume_u8 (buf + i, buf + len, &ptr->form, &i))) {
			free (ptr);
			return ret;
		}

		if (!(consume_u32 (buf + i, buf + len, &ptr->param_count, &i))) {
			free (ptr);
			return ret;
		}

		if (!(i + ptr->param_count < len)) {
			free (ptr);
			return ret;
		}

		int j;
		for (j = 0; j < ptr->param_count; j++) {
			if (!(consume_s8 (buf + i, buf + len, (st8*)&ptr->param_types[j], &i))) {
				free (ptr);
				return ret;
			}
		}

		if (!(consume_s8 (buf + i, buf + len, &ptr->return_count, &i))) {
			free (ptr);
			return ret;
		}

		if (ptr->return_count > 1) {
			free(ptr);
			return ret;
		}

		if (ptr->return_count == 1) {
			if (!(consume_s8 (buf + i, buf + len, (st8*)&ptr->return_type, &i))) {
				free(ptr);
				return ret;
			}
		}

		ptr->to_str = r_bin_wasm_type_entry_to_string (ptr);

		r_list_append (ret, ptr);

		r += 1;

	}

	return ret;

}
