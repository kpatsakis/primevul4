free_Digest(struct _7z_digests *d)
{
	free(d->defineds);
	free(d->digests);
}
