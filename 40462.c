static inline int blkcipher_next_slow(struct blkcipher_desc *desc,
				      struct blkcipher_walk *walk,
				      unsigned int bsize,
				      unsigned int alignmask)
{
	unsigned int n;
	unsigned aligned_bsize = ALIGN(bsize, alignmask + 1);

	if (walk->buffer)
		goto ok;

	walk->buffer = walk->page;
	if (walk->buffer)
		goto ok;

	n = aligned_bsize * 3 - (alignmask + 1) +
	    (alignmask & ~(crypto_tfm_ctx_alignment() - 1));
	walk->buffer = kmalloc(n, GFP_ATOMIC);
	if (!walk->buffer)
		return blkcipher_walk_done(desc, walk, -ENOMEM);

ok:
	walk->dst.virt.addr = (u8 *)ALIGN((unsigned long)walk->buffer,
					  alignmask + 1);
	walk->dst.virt.addr = blkcipher_get_spot(walk->dst.virt.addr, bsize);
	walk->src.virt.addr = blkcipher_get_spot(walk->dst.virt.addr +
						 aligned_bsize, bsize);

	scatterwalk_copychunks(walk->src.virt.addr, &walk->in, bsize, 0);

	walk->nbytes = bsize;
	walk->flags |= BLKCIPHER_WALK_SLOW;

	return 0;
}
