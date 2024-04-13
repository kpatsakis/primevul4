ut32 r_bin_wasm_get_entrypoint (RBinWasmObj *bin) {
	RList *secs = NULL;
	RBinWasmStartEntry *start = NULL;
	RBinWasmSection *sec = NULL;
	RBinWasmCodeEntry *func = NULL;

	if (!bin || !bin->g_sections) {
		return 0;
	}
	if (bin->entrypoint) {
		return bin->entrypoint;
	}
	if (bin->g_start) {
		start = bin->g_start;
	} else if (!(secs = r_bin_wasm_get_sections_by_id (bin->g_sections, R_BIN_WASM_SECTION_START))) {
		return 0;
	} else if (!(sec = (RBinWasmSection*) r_list_first (secs))) {
		return 0;
	} else {
		start = r_bin_wasm_get_start (bin, sec);
		bin->g_start = start;
	}

	if (!start) {
		return 0;
	}

	func = r_list_get_n (r_bin_wasm_get_codes (bin), start->index);
	return (ut32)func? func->code: 0;

}
