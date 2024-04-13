int digest_algo_register(struct digest_algo *d)
{
	if (!d || !d->base.name || !d->update || !d->final || !d->verify)
		return -EINVAL;

	if (!d->init)
		d->init = dummy_init;

	if (!d->alloc)
		d->alloc = dummy_init;

	if (!d->free)
		d->free = dummy_free;

	list_add_tail(&d->list, &digests);

	return 0;
}