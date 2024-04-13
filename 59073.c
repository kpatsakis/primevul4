static char *dex_class_name(RBinDexObj *bin, RBinDexClass *c) {
	return dex_class_name_byid (bin, c->class_id);
}
