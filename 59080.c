static char *dex_method_name(RBinDexObj *bin, int idx) {
	if (idx < 0 || idx >= bin->header.method_size) {
		return NULL;
	}
	int cid = bin->methods[idx].class_id;
	if (cid < 0 || cid >= bin->header.strings_size) {
		return NULL;
	}
	int tid = bin->methods[idx].name_id;
	if (tid < 0 || tid >= bin->header.strings_size) {
		return NULL;
	}
	return getstr (bin, tid);
}
