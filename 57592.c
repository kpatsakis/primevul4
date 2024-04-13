static int skcipher_copy_iv(struct skcipher_walk *walk)
{
	unsigned a = crypto_tfm_ctx_alignment() - 1;
	unsigned alignmask = walk->alignmask;
	unsigned ivsize = walk->ivsize;
	unsigned bs = walk->stride;
	unsigned aligned_bs;
	unsigned size;
	u8 *iv;

	aligned_bs = ALIGN(bs, alignmask);

	/* Minimum size to align buffer by alignmask. */
	size = alignmask & ~a;

	if (walk->flags & SKCIPHER_WALK_PHYS)
		size += ivsize;
	else {
		size += aligned_bs + ivsize;

		/* Minimum size to ensure buffer does not straddle a page. */
		size += (bs - 1) & ~(alignmask | a);
	}

	walk->buffer = kmalloc(size, skcipher_walk_gfp(walk));
	if (!walk->buffer)
		return -ENOMEM;

	iv = PTR_ALIGN(walk->buffer, alignmask + 1);
	iv = skcipher_get_spot(iv, bs) + aligned_bs;

	walk->iv = memcpy(iv, walk->iv, walk->ivsize);
	return 0;
}
