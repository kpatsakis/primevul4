void r_bin_wasm_destroy (RBinFile *arch) {
	RBinWasmObj *bin;

	if (!arch || !arch->o || !arch->o->bin_obj) {
		return;
	}

	bin = arch->o->bin_obj;
	r_buf_free (bin->buf);

	r_list_free (bin->g_sections);
	r_list_free (bin->g_types);

	r_list_free (bin->g_imports);
	r_list_free (bin->g_exports);
	r_list_free (bin->g_tables);
	r_list_free (bin->g_memories);
	r_list_free (bin->g_globals);
	r_list_free (bin->g_codes);
	r_list_free (bin->g_datas);

	free (bin->g_start);
	free (bin);
	arch->o->bin_obj = NULL;
}
