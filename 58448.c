RList *r_bin_wasm_get_codes (RBinWasmObj *bin) {
	RBinWasmSection *code = NULL;;
	RList *codes = NULL;

	if (!bin || !bin->g_sections) {
		return NULL;
	}

	if (bin->g_codes) {
		return bin->g_codes;
	}

	if (!(codes = r_bin_wasm_get_sections_by_id (bin->g_sections,
						R_BIN_WASM_SECTION_CODE))) {
		return r_list_new();
	}

	if (!(code = (RBinWasmSection*) r_list_first (codes))) {
		return r_list_new();
	}

	bin->g_codes = r_bin_wasm_get_code_entries (bin, code);

	return bin->g_codes;
}
