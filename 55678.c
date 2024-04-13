checksum_update(struct archive_read *a, const void *abuff, size_t asize,
    const void *ebuff, size_t esize)
{
	struct xar *xar;

	xar = (struct xar *)(a->format->data);
	_checksum_update(&(xar->a_sumwrk), abuff, asize);
	_checksum_update(&(xar->e_sumwrk), ebuff, esize);
}
