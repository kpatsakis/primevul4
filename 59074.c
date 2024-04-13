static char *dex_class_name_byid(RBinDexObj *bin, int cid) {
	int tid;
	if (!bin || !bin->types) {
		return NULL;
	}
	if (cid < 0 || cid >= bin->header.types_size) {
		return NULL;
	}
	tid = bin->types[cid].descriptor_id;
	return getstr (bin, tid);
}
