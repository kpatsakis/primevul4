static int ablkcipher_walk_first(struct ablkcipher_request *req,
				 struct ablkcipher_walk *walk)
{
	struct crypto_tfm *tfm = req->base.tfm;
	unsigned int alignmask;

	alignmask = crypto_tfm_alg_alignmask(tfm);
	if (WARN_ON_ONCE(in_irq()))
		return -EDEADLK;

	walk->nbytes = walk->total;
	if (unlikely(!walk->total))
		return 0;

	walk->iv_buffer = NULL;
	walk->iv = req->info;
	if (unlikely(((unsigned long)walk->iv & alignmask))) {
		int err = ablkcipher_copy_iv(walk, tfm, alignmask);
		if (err)
			return err;
	}

	scatterwalk_start(&walk->in, walk->in.sg);
	scatterwalk_start(&walk->out, walk->out.sg);

	return ablkcipher_walk_next(req, walk);
}
