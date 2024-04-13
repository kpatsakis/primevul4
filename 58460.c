RList *r_bin_wasm_get_tables (RBinWasmObj *bin) {
	RBinWasmSection *table = NULL;
	RList *tables = NULL;

	if (!bin || !bin->g_sections) {
		return NULL;
	}

	if (bin->g_tables) {
		return bin->g_tables;
	}

	if (!(tables = r_bin_wasm_get_sections_by_id (bin->g_sections,
						R_BIN_WASM_SECTION_TABLE))) {
		return r_list_new();
	}

	if (!(table = (RBinWasmSection*) r_list_first (tables))) {
		r_list_free (tables);
		return r_list_new();
	}

	bin->g_tables = r_bin_wasm_get_table_entries (bin, table);

	r_list_free (tables);
	return bin->g_tables;
}
