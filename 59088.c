static bool is_class_idx_in_code_classes(RBinDexObj *bin, int class_idx) {
	int i;
	for (i = 0; i < bin->header.class_size; i++) {
		if (class_idx == bin->classes[i].class_id) {
			return true;
		}
	}
	return false;
}
