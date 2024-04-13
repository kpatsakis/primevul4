static inline void ablkcipher_buffer_write(struct ablkcipher_buffer *p)
{
	scatterwalk_copychunks(p->data, &p->dst, p->len, 1);
}
