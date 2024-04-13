int ablkcipher_walk_done(struct ablkcipher_request *req,
			 struct ablkcipher_walk *walk, int err)
{
	struct crypto_tfm *tfm = req->base.tfm;
	unsigned int nbytes = 0;

	if (likely(err >= 0)) {
		unsigned int n = walk->nbytes - err;

		if (likely(!(walk->flags & ABLKCIPHER_WALK_SLOW)))
			n = ablkcipher_done_fast(walk, n);
		else if (WARN_ON(err)) {
			err = -EINVAL;
			goto err;
		} else
			n = ablkcipher_done_slow(walk, n);

		nbytes = walk->total - n;
		err = 0;
	}

	scatterwalk_done(&walk->in, 0, nbytes);
	scatterwalk_done(&walk->out, 1, nbytes);

err:
	walk->total = nbytes;
	walk->nbytes = nbytes;

	if (nbytes) {
		crypto_yield(req->base.flags);
		return ablkcipher_walk_next(req, walk);
	}

	if (walk->iv != req->info)
		memcpy(req->info, walk->iv, tfm->crt_ablkcipher.ivsize);
	kfree(walk->iv_buffer);

	return err;
}
