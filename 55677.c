checksum_init(struct archive_read *a, int a_sum_alg, int e_sum_alg)
{
	struct xar *xar;

	xar = (struct xar *)(a->format->data);
	_checksum_init(&(xar->a_sumwrk), a_sum_alg);
	_checksum_init(&(xar->e_sumwrk), e_sum_alg);
}
