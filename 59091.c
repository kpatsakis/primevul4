static RList *methods(RBinFile *arch) {
	RBinDexObj *bin;
	if (!arch || !arch->o || !arch->o->bin_obj) {
		return NULL;
	}
	bin = (RBinDexObj*) arch->o->bin_obj;
	if (!bin->methods_list) {
		dex_loadcode (arch, bin);
	}
	return bin->methods_list;
}
