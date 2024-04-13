static RList* imports(RBinFile *arch) {
	RBinDexObj *bin = (RBinDexObj*) arch->o->bin_obj;
	if (!bin) {
		return NULL;
	}
	if (bin && bin->imports_list) {
		return bin->imports_list;
	}
	dex_loadcode (arch, bin);
	return bin->imports_list;
}
