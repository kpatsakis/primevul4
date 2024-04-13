struct digest *digest_alloc(const char *name)
{
	struct digest *d;
	struct digest_algo *algo;

	algo = digest_algo_get_by_name(name);
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