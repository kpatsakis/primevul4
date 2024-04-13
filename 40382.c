static inline int ablkcipher_next_slow(struct ablkcipher_request *req,
				       struct ablkcipher_walk *walk,
				       unsigned int bsize,
				       unsigned int alignmask,
				       void **src_p, void **dst_p)
{
	unsigned aligned_bsize = ALIGN(bsize, alignmask + 1);
	struct ablkcipher_buffer *p;
	void *src, *dst, *base;
	unsigned int n;

	n = ALIGN(sizeof(struct ablkcipher_buffer), alignmask + 1);
	n += (aligned_bsize * 3 - (alignmask + 1) +
	      (alignmask & ~(crypto_tfm_ctx_alignment() - 1)));

	p = kmalloc(n, GFP_ATOMIC);
	if (!p)
		return ablkcipher_walk_done(req, walk, -ENOMEM);

	base = p + 1;

	dst = (u8 *)ALIGN((unsigned long)base, alignmask + 1);
	src = dst = ablkcipher_get_spot(dst, bsize);

	p->len = bsize;
	p->data = dst;

	scatterwalk_copychunks(src, &walk->in, bsize, 0);

	ablkcipher_queue_write(walk, p);

	walk->nbytes = bsize;
	walk->flags |= ABLKCIPHER_WALK_SLOW;

	*src_p = src;
	*dst_p = dst;

	return 0;
}
