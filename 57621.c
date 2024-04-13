static int skcipher_walk_first(struct skcipher_walk *walk)
{
	walk->nbytes = 0;

	if (WARN_ON_ONCE(in_irq()))
		return -EDEADLK;

	if (unlikely(!walk->total))
		return 0;

	walk->buffer = NULL;
	if (unlikely(((unsigned long)walk->iv & walk->alignmask))) {
		int err = skcipher_copy_iv(walk);
		if (err)
			return err;
	}

	walk->page = NULL;
	walk->nbytes = walk->total;

	return skcipher_walk_next(walk);
}
