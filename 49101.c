static unsigned int __ctrblk_init(u8 *ctrptr, unsigned int nbytes)
{
	unsigned int i, n;

	/* align to block size, max. PAGE_SIZE */
	n = (nbytes > PAGE_SIZE) ? PAGE_SIZE : nbytes & ~(DES_BLOCK_SIZE - 1);
	for (i = DES_BLOCK_SIZE; i < n; i += DES_BLOCK_SIZE) {
		memcpy(ctrptr + i, ctrptr + i - DES_BLOCK_SIZE, DES_BLOCK_SIZE);
		crypto_inc(ctrptr + i, DES_BLOCK_SIZE);
	}
	return n;
}
