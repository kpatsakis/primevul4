static int skcipher_next_slow(struct skcipher_walk *walk, unsigned int bsize)
{
	bool phys = walk->flags & SKCIPHER_WALK_PHYS;
	unsigned alignmask = walk->alignmask;
	struct skcipher_walk_buffer *p;
	unsigned a;
	unsigned n;
	u8 *buffer;
	void *v;

	if (!phys) {
		if (!walk->buffer)
			walk->buffer = walk->page;
		buffer = walk->buffer;
		if (buffer)
			goto ok;
	}

	/* Start with the minimum alignment of kmalloc. */
	a = crypto_tfm_ctx_alignment() - 1;
	n = bsize;

	if (phys) {
		/* Calculate the minimum alignment of p->buffer. */
		a &= (sizeof(*p) ^ (sizeof(*p) - 1)) >> 1;
		n += sizeof(*p);
	}

	/* Minimum size to align p->buffer by alignmask. */
	n += alignmask & ~a;

	/* Minimum size to ensure p->buffer does not straddle a page. */
	n += (bsize - 1) & ~(alignmask | a);

	v = kzalloc(n, skcipher_walk_gfp(walk));
	if (!v)
		return skcipher_walk_done(walk, -ENOMEM);

	if (phys) {
		p = v;
		p->len = bsize;
		skcipher_queue_write(walk, p);
		buffer = p->buffer;
	} else {
		walk->buffer = v;
		buffer = v;
	}

ok:
	walk->dst.virt.addr = PTR_ALIGN(buffer, alignmask + 1);
	walk->dst.virt.addr = skcipher_get_spot(walk->dst.virt.addr, bsize);
	walk->src.virt.addr = walk->dst.virt.addr;

	scatterwalk_copychunks(walk->src.virt.addr, &walk->in, bsize, 0);

	walk->nbytes = bsize;
	walk->flags |= SKCIPHER_WALK_SLOW;

	return 0;
}
