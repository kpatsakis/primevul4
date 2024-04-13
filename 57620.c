int skcipher_walk_done(struct skcipher_walk *walk, int err)
{
	unsigned int n = walk->nbytes - err;
	unsigned int nbytes;

	nbytes = walk->total - n;

	if (unlikely(err < 0)) {
		nbytes = 0;
		n = 0;
	} else if (likely(!(walk->flags & (SKCIPHER_WALK_PHYS |
					   SKCIPHER_WALK_SLOW |
					   SKCIPHER_WALK_COPY |
					   SKCIPHER_WALK_DIFF)))) {
unmap_src:
		skcipher_unmap_src(walk);
	} else if (walk->flags & SKCIPHER_WALK_DIFF) {
		skcipher_unmap_dst(walk);
		goto unmap_src;
	} else if (walk->flags & SKCIPHER_WALK_COPY) {
		skcipher_map_dst(walk);
		memcpy(walk->dst.virt.addr, walk->page, n);
		skcipher_unmap_dst(walk);
	} else if (unlikely(walk->flags & SKCIPHER_WALK_SLOW)) {
		if (WARN_ON(err)) {
			err = -EINVAL;
			nbytes = 0;
		} else
			n = skcipher_done_slow(walk, n);
	}

	if (err > 0)
		err = 0;

	walk->total = nbytes;
	walk->nbytes = nbytes;

	scatterwalk_advance(&walk->in, n);
	scatterwalk_advance(&walk->out, n);
	scatterwalk_done(&walk->in, 0, nbytes);
	scatterwalk_done(&walk->out, 1, nbytes);

	if (nbytes) {
		crypto_yield(walk->flags & SKCIPHER_WALK_SLEEP ?
			     CRYPTO_TFM_REQ_MAY_SLEEP : 0);
		return skcipher_walk_next(walk);
	}

	/* Short-circuit for the common/fast path. */
	if (!((unsigned long)walk->buffer | (unsigned long)walk->page))
		goto out;

	if (walk->flags & SKCIPHER_WALK_PHYS)
		goto out;

	if (walk->iv != walk->oiv)
		memcpy(walk->oiv, walk->iv, walk->ivsize);
	if (walk->buffer != walk->page)
		kfree(walk->buffer);
	if (walk->page)
		free_page((unsigned long)walk->page);

out:
	return err;
}
