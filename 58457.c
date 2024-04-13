static RList *r_bin_wasm_get_sections_by_id (RList *sections, ut8 id) {
	RBinWasmSection *sec = NULL;
	RList *ret = NULL;	
	RListIter *iter = NULL;

	if (!(ret = r_list_new ())) {
		return NULL;
	}
	r_list_foreach (sections, iter, sec) {
		if (sec->id == id) {
			r_list_append(ret, sec);
		}
	}
	return ret;
}
