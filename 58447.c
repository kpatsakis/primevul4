static RList *r_bin_wasm_get_code_entries (RBinWasmObj *bin, RBinWasmSection *sec) {
	RList *ret = NULL;
	RBinWasmCodeEntry *ptr = NULL;

	if (!(ret = r_list_newf ((RListFree)free))) {
		return NULL;
	}

	ut8* buf = bin->buf->buf + (ut32)sec->payload_data;
	ut32 len =  sec->payload_len;
	ut32 count = sec->count;
	ut32 i = 0, j = 0, r = 0;
	size_t n = 0;

	while (i < len && r < count) {

		if (!(ptr = R_NEW0 (RBinWasmCodeEntry))) {
			return ret;
		}

		if (!(n = consume_u32 (buf + i, buf + len, &ptr->body_size, &i))) {
			free (ptr);
			return ret;
		}

		if (!(i + ptr->body_size - 1 < len)) {
			free (ptr);
			return ret;
		}

		j = i;

		if (!(n = consume_u32 (buf + i, buf + len, &ptr->local_count, &i))) {
			free (ptr);
			return ret;
		}

		if ((n = consume_locals (buf + i, buf + len, ptr->local_count,ptr, &i)) < ptr->local_count) {
			free (ptr);
			return ret;
		}

		ptr->code = sec->payload_data + i;
		ptr->len = ptr->body_size - (i - j);

		i += ptr->len - 1; // consume bytecode

		if (!(consume_u8 (buf + i, buf + len, &ptr->byte, &i))) {
			free (ptr);
			return ret;
		}

		if (ptr->byte != R_BIN_WASM_END_OF_CODE) {
			free (ptr);
			return ret;
		}


		r_list_append (ret, ptr);

		r += 1;

	}

	return ret;
 }
