static int ablkcipher_walk_next(struct ablkcipher_request *req,
				struct ablkcipher_walk *walk)
{
	struct crypto_tfm *tfm = req->base.tfm;
	unsigned int alignmask, bsize, n;
	void *src, *dst;
	int err;

	alignmask = crypto_tfm_alg_alignmask(tfm);
	n = walk->total;
	if (unlikely(n < crypto_tfm_alg_blocksize(tfm))) {
		req->base.flags |= CRYPTO_TFM_RES_BAD_BLOCK_LEN;
		return ablkcipher_walk_done(req, walk, -EINVAL);
	}

	walk->flags &= ~ABLKCIPHER_WALK_SLOW;
	src = dst = NULL;

	bsize = min(walk->blocksize, n);
	n = scatterwalk_clamp(&walk->in, n);
	n = scatterwalk_clamp(&walk->out, n);

	if (n < bsize ||
	    !scatterwalk_aligned(&walk->in, alignmask) ||
	    !scatterwalk_aligned(&walk->out, alignmask)) {
		err = ablkcipher_next_slow(req, walk, bsize, alignmask,
					   &src, &dst);
		goto set_phys_lowmem;
	}

	walk->nbytes = n;

	return ablkcipher_next_fast(req, walk);

set_phys_lowmem:
	if (err >= 0) {
		walk->src.page = virt_to_page(src);
		walk->dst.page = virt_to_page(dst);
		walk->src.offset = ((unsigned long)src & (PAGE_SIZE - 1));
		walk->dst.offset = ((unsigned long)dst & (PAGE_SIZE - 1));
	}

	return err;
}
