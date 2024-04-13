static size_t consume_s8 (ut8 *buf, ut8 *max, st8 *out, ut32 *offset) {
	size_t n;
	ut32 tmp;
	if (!(n = consume_u32 (buf, max, &tmp, offset)) || n > 1) {
		return 0;
	}
	*out = (st8)(tmp & 0x7f);
	return 1;	
}
