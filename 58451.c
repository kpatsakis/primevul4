RList *r_bin_wasm_get_exports (RBinWasmObj *bin) {

	RBinWasmSection *export = NULL;
	RList *exports = NULL;

	if (!bin || !bin->g_sections) {
		return NULL;
	}

	if (bin->g_exports) {
		return bin->g_exports;
	}

	if (!(exports= r_bin_wasm_get_sections_by_id (bin->g_sections,
						R_BIN_WASM_SECTION_EXPORT))) {
		return r_list_new();
	}

	if (!(export = (RBinWasmSection*) r_list_first (exports))) {
		return r_list_new();
	}

	bin->g_exports = r_bin_wasm_get_export_entries (bin, export);

	return bin->g_exports;
}
