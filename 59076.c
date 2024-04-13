static char *dex_field_name(RBinDexObj *bin, int fid) {
	int cid, tid, type_id;
	if (!bin || !bin->fields) {
		return NULL;
	}
	if (fid < 0 || fid >= bin->header.fields_size) {
		return NULL;
	}
	cid = bin->fields[fid].class_id;
	if (cid < 0 || cid >= bin->header.types_size) {
		return NULL;
	}
	type_id = bin->fields[fid].type_id;
	if (type_id < 0 || type_id >= bin->header.types_size) {
		return NULL;
	}
	tid = bin->fields[fid].name_id;
	return r_str_newf ("%s->%s %s", getstr (bin, bin->types[cid].descriptor_id),
		getstr (bin, tid), getstr (bin, bin->types[type_id].descriptor_id));
}
