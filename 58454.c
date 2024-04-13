RList *r_bin_wasm_get_memories (RBinWasmObj *bin) {
	RBinWasmSection *memory;
	RList *memories;

	if (!bin || !bin->g_sections) {
		return NULL;
	}

	if (bin->g_memories) {
		return bin->g_memories;
	}

	if (!(memories = r_bin_wasm_get_sections_by_id (bin->g_sections,
						R_BIN_WASM_SECTION_MEMORY))) {
		return r_list_new();
	}

	if (!(memory = (RBinWasmSection*) r_list_first (memories))) {
		return r_list_new();
	}

	bin->g_memories = r_bin_wasm_get_memory_entries (bin, memory);

	return bin->g_memories;
}
