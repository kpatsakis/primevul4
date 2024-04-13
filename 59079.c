static char *dex_method_fullname(RBinDexObj *bin, int method_idx) {
	if (!bin || !bin->types) {
		return NULL;
	}
	if (method_idx < 0 || method_idx >= bin->header.method_size) {
		return NULL;
	}
	int cid = bin->methods[method_idx].class_id;
	if (cid < 0 || cid >= bin->header.types_size) {
		return NULL;
	}
	char *name = dex_method_name (bin, method_idx);
	char *class_name = dex_class_name_byid (bin, cid);
	class_name = r_str_replace (class_name, ";", "", 0); //TODO: move to func
	char *signature = dex_method_signature (bin, method_idx);
	char *flagname = r_str_newf ("%s.%s%s", class_name, name, signature);
	free (name);
	free (class_name);
	free (signature);
	return flagname;
}
