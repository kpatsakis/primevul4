static int check_bytes(const ut8 *buf, ut64 length) {
	return length > 4 && !memcmp (buf, "bFLT", 4);
}
