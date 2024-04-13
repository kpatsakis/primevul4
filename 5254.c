struct digest *digest_alloc_by_algo(enum hash_algo hash_algo)
{
	struct digest *d;
	struct digest_algo *algo;

	algo = digest_algo_get_by_algo(hash_algo);
	if (!algo)
		return NULL;

	d = xzalloc(sizeof(*d));
	d->algo = algo;
	d->ctx = xzalloc(algo->ctx_length);
	if (d->algo->alloc(d)) {
		digest_free(d);
		return NULL;
	}

	return d;
}