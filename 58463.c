RBinWasmObj *r_bin_wasm_init (RBinFile *arch) {
	RBinWasmObj *bin = R_NEW0 (RBinWasmObj);
	if (!bin) {
		return NULL;
	}
	if (!(bin->buf = r_buf_new ())) {
		free (bin);
		return NULL;
	}
	bin->size = (ut32)arch->buf->length;
	if (!r_buf_set_bytes (bin->buf, arch->buf->buf, bin->size)) {
		r_bin_wasm_destroy (arch);
		free (bin);
		return NULL;
	}

	bin->g_sections = r_bin_wasm_get_sections (bin);

	bin->g_types = r_bin_wasm_get_types (bin);
	bin->g_imports = r_bin_wasm_get_imports (bin);	
	bin->g_exports = r_bin_wasm_get_exports (bin);
	bin->g_tables = r_bin_wasm_get_tables (bin);
	bin->g_memories = r_bin_wasm_get_memories (bin);
	bin->g_globals = r_bin_wasm_get_globals (bin);
	bin->g_codes = r_bin_wasm_get_codes (bin);
	bin->g_datas = r_bin_wasm_get_datas (bin);

	bin->entrypoint = r_bin_wasm_get_entrypoint (bin);

	return bin;
}
