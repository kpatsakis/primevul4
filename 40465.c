static int blkcipher_walk_first(struct blkcipher_desc *desc,
				struct blkcipher_walk *walk)
{
	struct crypto_blkcipher *tfm = desc->tfm;
	unsigned int alignmask = crypto_blkcipher_alignmask(tfm);

	if (WARN_ON_ONCE(in_irq()))
		return -EDEADLK;

	walk->nbytes = walk->total;
	if (unlikely(!walk->total))
		return 0;

	walk->buffer = NULL;
	walk->iv = desc->info;
	if (unlikely(((unsigned long)walk->iv & alignmask))) {
		int err = blkcipher_copy_iv(walk, tfm, alignmask);
		if (err)
			return err;
	}

	scatterwalk_start(&walk->in, walk->in.sg);
	scatterwalk_start(&walk->out, walk->out.sg);
	walk->page = NULL;

	return blkcipher_walk_next(desc, walk);
}
