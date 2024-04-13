RList *r_bin_wasm_get_types (RBinWasmObj *bin) {
	RBinWasmSection *type = NULL;
	RList *types = NULL;

	if (!bin || !bin->g_sections) {
		return NULL;
	}

	if (bin->g_types) {
		return bin->g_types;
	}

	if (!(types = r_bin_wasm_get_sections_by_id (bin->g_sections,
						R_BIN_WASM_SECTION_TYPE))) {
		return r_list_new();
	}

	if (!(type = (RBinWasmSection*) r_list_first (types))) {
		return r_list_new();
	}

	bin->g_types = r_bin_wasm_get_type_entries (bin, type);

	return bin->g_types;
}
