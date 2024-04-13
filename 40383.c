static inline void ablkcipher_queue_write(struct ablkcipher_walk *walk,
					  struct ablkcipher_buffer *p)
{
	p->dst = walk->out;
	list_add_tail(&p->entry, &walk->buffers);
}
