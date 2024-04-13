static void __r_bin_class_free(RBinClass *p) {
	r_list_free (p->methods);
	r_list_free (p->fields);
	r_bin_class_free (p);
}
