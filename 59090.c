static void *load_bytes(RBinFile *arch, const ut8 *buf, ut64 sz, ut64 loadaddr, Sdb *sdb){
	void *res = NULL;
	RBuffer *tbuf = NULL;
	if (!buf || !sz || sz == UT64_MAX) {
		return NULL;
	}
	tbuf = r_buf_new ();
	if (!tbuf) {
		return NULL;
	}		 
	r_buf_set_bytes (tbuf, buf, sz);
	res = r_bin_dex_new_buf (tbuf);
	r_buf_free (tbuf);
	return res;
}
