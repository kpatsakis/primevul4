static ut64 size(RBinFile *arch) {
	int ret;
	ut32 off = 0, len = 0;
	ut8 u32s[sizeof (ut32)] = {0};

	ret = r_buf_read_at (arch->buf, 108, u32s, 4);
	if (ret != 4) {
		return 0;
	}
	off = r_read_le32 (u32s);
	ret = r_buf_read_at (arch->buf, 104, u32s, 4);
	if (ret != 4) {
		return 0;
	}
	len = r_read_le32 (u32s);
	return off + len;
}
