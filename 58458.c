static RBinWasmStartEntry *r_bin_wasm_get_start (RBinWasmObj *bin, RBinWasmSection *sec) {

	RBinWasmStartEntry *ptr;	

	if (!(ptr = R_NEW0 (RBinWasmStartEntry))) {
		return NULL;
	}

	ut8* buf = bin->buf->buf + (ut32)sec->payload_data;
	ut32 len =  sec->payload_len;
	ut32 i = 0;

	if (!(consume_u32 (buf + i, buf + len, &ptr->index, &i))) {
		free (ptr);
		return NULL;
	}

	return ptr;

}
