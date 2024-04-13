static inline int noodle(ELFOBJ *bin, const char *s) {
	const ut8 *p = bin->b->buf;
	if (bin->b->length > 64)  {
		p += bin->b->length - 64;
	} else {
		return 0;
	}
	return r_mem_mem (p, 64, (const ut8 *)s, strlen (s)) != NULL;
}
