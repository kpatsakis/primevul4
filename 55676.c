checksum_cleanup(struct archive_read *a) {
	struct xar *xar;

	xar = (struct xar *)(a->format->data);

	_checksum_final(&(xar->a_sumwrk), NULL, 0);
	_checksum_final(&(xar->e_sumwrk), NULL, 0);
}
