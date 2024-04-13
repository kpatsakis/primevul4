static void skcipher_queue_write(struct skcipher_walk *walk,
				 struct skcipher_walk_buffer *p)
{
	p->dst = walk->out;
	list_add_tail(&p->entry, &walk->buffers);
}
