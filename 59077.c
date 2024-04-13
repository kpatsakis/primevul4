static ut64 dex_get_type_offset(RBinFile *arch, int type_idx) {
	RBinDexObj *bin = (RBinDexObj*) arch->o->bin_obj;
	if (!bin || !bin->types) {
		return 0;
	}
	if (type_idx < 0 || type_idx >= bin->header.types_size) {
		return 0;
	}
	return bin->header.types_offset + type_idx * 0x04; //&bin->types[type_idx];
}
