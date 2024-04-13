RList *r_bin_wasm_get_imports (RBinWasmObj *bin) {
	RBinWasmSection *import = NULL;
	RList *imports = NULL;

	if (!bin || !bin->g_sections) {
		return NULL;
	}
	if (bin->g_imports) {
		return bin->g_imports;
	}
	if (!(imports = r_bin_wasm_get_sections_by_id (bin->g_sections,
						R_BIN_WASM_SECTION_IMPORT))) {
		return r_list_new();
	}
	if (!(import = (RBinWasmSection*) r_list_first (imports))) {
		return r_list_new();
	}
	return bin->g_imports = r_bin_wasm_get_import_entries (bin, import);
}
