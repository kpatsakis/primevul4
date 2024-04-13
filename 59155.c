static int check(RBinFile *arch) {
	const ut8 *bytes = arch ? r_buf_buffer (arch->buf) : NULL;
	ut64 sz = arch ? r_buf_size (arch->buf): 0; 
	if (!bytes || !sz) {
		return false;
	}
	return check_bytes (bytes, sz);
}
