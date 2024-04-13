RList *r_bin_wasm_get_datas (RBinWasmObj *bin) {
	RBinWasmSection *data = NULL;
	RList *datas = NULL;

	if (!bin || !bin->g_sections) {
		return NULL;
	}

	if (bin->g_datas) {
		return bin->g_datas;
	}

	if (!(datas = r_bin_wasm_get_sections_by_id (bin->g_sections,
						R_BIN_WASM_SECTION_DATA))) {
		return r_list_new();
	}

	if (!(data = (RBinWasmSection*) r_list_first (datas))) {
		return r_list_new();
	}

	bin->g_datas = r_bin_wasm_get_data_entries (bin, data);

	return bin->g_datas;
}
