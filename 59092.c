static ut64 offset_of_method_idx(RBinFile *arch, struct r_bin_dex_obj_t *dex, int idx) {
	ut64 off = dex->header.method_offset + idx;
	off = sdb_num_get (mdb, sdb_fmt (0, "method.%d", idx), 0);
	return (ut64) off;
}
