void digest_free(struct digest *d)
{
	if (!d)
		return;
	d->algo->free(d);
	free(d->ctx);
	free(d);
}