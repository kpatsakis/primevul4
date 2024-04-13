void digest_algo_unregister(struct digest_algo *d)
{
	if (!d)
		return;

	list_del(&d->list);
}