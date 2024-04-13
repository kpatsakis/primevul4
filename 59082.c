static Sdb *get_sdb (RBinObject *o) {
	if (!o || !o->bin_obj) {
		return NULL;
	}
	struct r_bin_dex_obj_t *bin = (struct r_bin_dex_obj_t *) o->bin_obj;
	if (bin->kv) {
		return bin->kv;
	}
	return NULL;
}
